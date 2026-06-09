#include "text_objects.h"

char get_brace_close(char open) {
  switch (open) {
  case '(':
    return ')';
  case '[':
    return ']';
  case '{':
    return '}';
  case '<':
    return '>';
  }
  return '\0';
}

// TODO: рассмотреть крайний случай, когда курсор на границе, и добавить модификатор around
struct Vec4 get_object_content_bounds(struct Document *doc, size_t count, char open, char close) {
  struct Vec3 left = doc->pos, right = doc->pos;
  size_t left_count = count, right_count = count;
  while (left_count || right_count) {
    struct Line *left_line = doc->buf[left.y], *right_line = doc->buf[right.y];

    if (left_line->buf[left.x] == close) left_count++;
    if (left_count && left_line->buf[left.x] == open) left_count--;
    if (left_count) {
      left.x--;
      if (left.x < 0) {
        if (!left.y) return (struct Vec4){-1};
        left.y--;
        left.x = MAX_LINE_X(doc->buf[left.y]->len);
      }
    }

    if (right_line->buf[right.x] == open) right_count++;
    if (right_count && right_line->buf[right.x] == close) right_count--;
    if (right_count) {
      right.x++;
      if (right.x >= right_line->len) {
        if (right.y == doc->len - 1) return (struct Vec4){-1};
        right.y++;
        right.x = 0;
      }
    }
  }

  if (left.x == doc->buf[left.y]->len - 1 && left.y < doc->len - 1) {
    left.y++;
    left.x = 0;
  } else {
    left.x++;
  }

  if (!right.x && right.y) {
    right.y--;
    right.x = MAX_LINE_X(doc->buf[right.y]->len);
  } else {
    right.x--;
  }

  return (struct Vec4){left.x, left.y, right.x, right.y};
}

struct Vec4 get_motion_object_bounds(struct Document *doc, enum EditorMode mode, struct DinamicMapping mapping) {
  if (mode == EDITOR_MODE_VISUAL) return (struct Vec4){doc->pos.x, doc->pos.y, doc->selected.x, doc->selected.y};
  size_t count = mapping.global_count;
  switch (mapping.motion_object) {
  case '(':
  case '{':
  case '[':
  case '<': {
    char close = get_brace_close(mapping.motion_object);
    if ((mapping.modifier != 'i' && mapping.modifier != 'a') || close == '\0') return (struct Vec4){-1};
    return get_object_content_bounds(doc, count, mapping.motion_object, close);
  }

  default:
    return (struct Vec4){-1};
  }
}
