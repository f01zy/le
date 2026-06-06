#include "text_objects.h"

enum WordGroup get_char_group(char ch) {
  if (ch == ' ' || ch == '\t' || ch == '\0') return WORD_GROUP_SPACE;
  if (IS_ALPHA(ch)) return WORD_GROUP_ALPHA;
  return WORD_GROUP_SPECIAL;
}

size_t get_certain_word_len(struct Line *line, enum WordGroup group, size_t start) {
  size_t curr = start;
  while (start < line->len && get_char_group(line->buf[curr]) == group) {
    curr++;
  }
  if (get_char_group(line->buf[curr]) != group) curr--;
  return curr - start + 1;
}

// TODO: немного багованая, надо починить
struct Vec4 get_word_bounds(struct Document *doc, size_t count) {
  struct Vec2 bounds = doc->pos;
  while (bounds.y < doc->len && count) {
    struct Line *line = doc->buf[bounds.y];
    if (!line->len) {
      bounds.y++;
      bounds.x = 0;
      count--;
      continue;
    }
    enum WordGroup group = get_char_group(line->buf[bounds.x]);
    size_t len = get_certain_word_len(line, group, bounds.x);
    bounds.x += len;
    if (bounds.x >= line->len) {
      if (bounds.y < doc->len - 1 && count) {
        bounds.y++;
        bounds.x = 0;
        count--;
        continue;
      }
    }
    count--;
  }
  bounds.x--;
  if (bounds.y && !bounds.x) {
    bounds.y--;
    bounds.x = doc->buf[bounds.y]->len;
  }
  return (struct Vec4){doc->pos.x, doc->pos.y, bounds.x, bounds.y};
}

struct Vec4 get_brace_bounds(struct Document *doc, size_t count, char brace_open, char brace_close) {
  struct Vec2 left = doc->pos, right = doc->pos;
  size_t left_count = count, right_count = count;
  while (left_count || right_count) {
    struct Line *line = doc->buf[left.y];

    if (left_count && line->buf[left.x] == brace_open) left_count--;
    if (left_count) {
      left.x--;
      if (left.x < 0) {
        left.y--;
        left.x = MAX_LINE_X(doc->buf[left.y]->len);
      }
    }

    if (right_count && line->buf[right.x] == brace_close) right_count--;
    if (right_count) {
      right.x++;
      if (right.x >= line->len) {
        right.y++;
        right.x = 0;
      }
    }
  }

  return (struct Vec4){left.x + 1, left.y, right.x - 1, right.y};
}

struct Vec4 get_motion_object_bounds(struct Document *doc, enum EditorMode mode, struct DinamicMapping mapping) {
  if (mode == EDITOR_MODE_VISUAL) return (struct Vec4){doc->pos.x, doc->pos.y, doc->selected.x, doc->selected.y};
  size_t count = mapping.global_count;
  switch (mapping.motion_object) {
  case 'w':
    if (mapping.modifier == 'i' || mapping.modifier == 'a') count = 1;
    return get_word_bounds(doc, count);

  case '(':
    if (mapping.modifier != 'i' && mapping.modifier != 'a') return (struct Vec4){-1};
    return get_brace_bounds(doc, count, '(', ')');

  case '{':
    if (mapping.modifier != 'i' && mapping.modifier != 'a') return (struct Vec4){-1};
    return get_brace_bounds(doc, count, '{', '}');

  default:
    return (struct Vec4){-1};
  }
}
