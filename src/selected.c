#include "selected.h"

void get_selected_coordinates(struct Vec4 *c) {
  int is_single_line = (c->ay == c->by);
  int minY = (c->ay > c->by) ? c->by : c->ay;
  int maxY = (c->ay > c->by) ? c->ay : c->by;
  int minX = is_single_line ? MIN(c->ax, c->bx) : ((c->ay > c->by) ? c->bx : c->ax);
  int maxX = is_single_line ? MAX(c->ax, c->bx) : ((c->ay > c->by) ? c->ax : c->bx);
  c->ay = minY, c->ax = minX;
  c->by = maxY, c->bx = maxX;
}

void get_selected_buffer(struct Document *doc, char *out, size_t max_size) {
  struct Vec4 c = {doc->pos.x, doc->pos.y, doc->selected.x, doc->selected.y};
  get_selected_coordinates(&c);
  struct Line *first = doc->buf[c.ay], *last = doc->buf[c.by];
  int curr = 0;

  if (c.ay != c.by) {
    int first_len = first->len - c.ax, last_len = c.bx + 1;
    xmemcpy(out, max_size - 1, first->buf + c.ax, first_len);
    curr += first_len;
    out[curr++] = '\n';
    for (int i = c.ay + 1; i < c.by; i++) {
      struct Line *line = doc->buf[i];
      xmemcpy(out + curr, max_size - curr - 1, line->buf, line->len);
      curr += line->len;
      out[curr++] = '\n';
    }
    xmemcpy(out + curr, max_size - curr - 1, last->buf, last_len);
    curr += last_len;
  } else {
    size_t len = c.bx - c.ax + 1;
    xmemcpy(out, max_size - 1, first->buf + c.ax, len);
    curr += len;
  }

  out[curr] = '\0';
}
