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

// TODO: починить remove_range, и, если надо, поправить эту функцию
struct Vec4 get_word_bounds(struct Document *doc, size_t count) {
  struct Vec2 bounds = {doc->pos.x, doc->pos.y};
  while (bounds.y < doc->len && count) {
    struct Line *line = doc->buf[bounds.y];
    enum WordGroup group = get_char_group(line->buf[bounds.x]);
    size_t len = get_certain_word_len(line, group, bounds.x);
    bounds.x += len;
    if (!--count) {
      bounds.x--;
      break;
    }
    if (count && bounds.x >= line->len) {
      bounds.x = 0;
      bounds.y++;
      count--;
    }
  }
  return (struct Vec4){doc->pos.x, doc->pos.y, bounds.x, bounds.y};
}

struct Vec4 get_motion_object_bounds(struct Document *doc, enum EditorMode mode, struct DinamicMapping mapping) {
  if (mode == EDITOR_MODE_VISUAL) return (struct Vec4){doc->pos.x, doc->pos.y, doc->selected.x, doc->selected.y};
  size_t count = mapping.global_count;
  switch (mapping.motion_object) {
  case 'w':
  case 'W':
    if (mapping.modifier == 'i' || mapping.modifier == 'a') count = 1;
    return get_word_bounds(doc, count);

  default:
    return (struct Vec4){-1};
  }
}
