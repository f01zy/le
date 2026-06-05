#include "text_objects.h"
#include <stdio.h>
#include <stdlib.h>

struct Vec4 get_motion_object_bounds(struct Document *doc, enum EditorMode mode, struct DinamicMapping mapping) {
  if (mode == EDITOR_MODE_VISUAL) return (struct Vec4){doc->pos.x, doc->pos.y, doc->selected.x, doc->selected.y};
  size_t count = mapping.global_count;
  switch (mapping.motion_object) {
  case 'w':
  case 'W':
    if (mapping.modifier == 'i' || mapping.modifier == 'a') count = 1;
    return get_word_bounds(doc, count, mapping.motion_object == 'W');
  default:
    return (struct Vec4){-1};
  }
}

struct Vec4 get_word_bounds(struct Document *doc, size_t count, bool is_big_word) {
  struct Vec2 bounds = {doc->pos.x, doc->pos.y};
  while (count && bounds.y < doc->len) {
    struct Line *line = doc->buf[bounds.y];
    while (bounds.x < line->len && (is_big_word ? line->buf[bounds.x] != ' ' : IS_ALPHA(line->buf[bounds.x]))) {
      bounds.x++;
    }
    if (bounds.x == line->len) {
      if (bounds.y == doc->len - 1) break;
      bounds.x = 0;
      bounds.y++;
    }
    count--;
  }
  return (struct Vec4){doc->pos.x, doc->pos.y, bounds.x, bounds.y};
}
