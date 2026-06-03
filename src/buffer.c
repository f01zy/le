#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"

void get_selected_coordinates(int *ay, int *ax, int *by, int *bx) {
  int is_single_line = (*ay == *by);
  int minY = (*ay > *by) ? *by : *ay;
  int maxY = (*ay > *by) ? *ay : *by;
  int minX = is_single_line ? MIN(*ax, *bx) : ((*ay > *by) ? *bx : *ax);
  int maxX = is_single_line ? MAX(*ax, *bx) : ((*ay > *by) ? *ax : *bx);
  *ay = minY, *ax = minX;
  *by = maxY, *bx = maxX;
}

size_t get_max_x(struct Line *line) { return line->len ? line->len - 1 : 0; }

struct Document *create_doc(struct Context *ctx) {
  struct Document *doc = (struct Document *)xcalloc(1, sizeof(struct Document));
  ctx->len++;
  if (ctx->len > ctx->size) {
    ctx->size = ctx->len;
    ctx->docs = (struct Document **)xrealloc(ctx->docs, ctx->size * sizeof(struct Document *));
  }
  add_line(doc, NULL, 0);
  ctx->docs[ctx->len - 1] = doc;
  return doc;
}

void set_doc_path(struct Document *doc, char *path) {
  if (!path) return;
  free(doc->path);
  char cwd[MAX_BUFFER_SIZE];
  if (!getcwd(cwd, sizeof(cwd))) return;
  char buf[MAX_BUFFER_SIZE];
  int len = path[0] == '/' ? snprintf(buf, sizeof(buf), "%s", path) : snprintf(buf, sizeof(buf), "%s/%s", cwd, path);
  doc->path = (char *)xmalloc(len + 1);
  memcpy(doc->path, buf, len);
  doc->path[len] = '\0';
}

void remove_doc_path(struct Document *doc) {
  free(doc->path);
  doc->path = NULL;
}

void add_line(struct Document *doc, char *data, int y) {
  if (y > doc->len) return;
  if (doc->len + 1 > doc->size) {
    doc->size = doc->len + 1 + ADDITIONAL_REALLOCATION;
    doc->buf = (struct Line **)xrealloc(doc->buf, doc->size * sizeof(struct Line *));
  }
  memmove(&doc->buf[y + 1], &doc->buf[y], (doc->len - y) * sizeof(struct Line *));
  struct Line *line = (struct Line *)xmalloc(sizeof(struct Line));
  int len = data ? strlen(data) : 0;
  line->buf = (char *)xmalloc(len + 1);
  line->len = len;
  line->size = len + 1;
  if (data) memcpy(line->buf, data, len);
  line->buf[line->len] = '\0';
  doc->buf[y] = line;
  doc->len++;
}

void remove_line(struct Document *doc, int y) {
  if (y >= doc->len) return;
  struct Line *line = doc->buf[y];
  free(line->buf);
  free(line);
  memmove(&doc->buf[y], &doc->buf[y + 1], (doc->len - y) * sizeof(struct Line *));
  doc->len--;
}

void write_to_line(struct Document *doc, int y, int x, char ch) {
  if (y >= doc->len) return;
  struct Line *line = doc->buf[y];
  if (x > line->len) return;
  if (line->len + 2 > line->size) {
    line->size = line->len + 1 + ADDITIONAL_REALLOCATION;
    line->buf = (char *)xrealloc(line->buf, line->size);
  }
  memmove(&line->buf[x + 1], &line->buf[x], line->len - x);
  line->len++;
  line->buf[x] = ch;
  line->buf[line->len] = '\0';
}

enum RemoveResult remove_from_line(struct Document *doc, int y, int x) {
  if (y >= doc->len || (!x && !y)) return REMOVE_NOTHING;
  struct Line *line = doc->buf[y];
  if (x > line->len) return REMOVE_NOTHING;
  if (x) {
    memmove(&line->buf[x - 1], &line->buf[x], line->len - x + 1);
    line->len--;
    line->buf[line->len] = '\0';
    return REMOVE_CHAR;
  }
  struct Line *prev = doc->buf[y - 1];
  if (line->len > 0) {
    int len = prev->len + line->len;
    if (prev->size - 1 < line->len) {
      prev->size = len + ADDITIONAL_REALLOCATION;
      prev->buf = (char *)xrealloc(prev->buf, prev->size);
    }
    memcpy(prev->buf + prev->len, line->buf, line->len);
    prev->len = len;
    prev->buf[len] = '\0';
  }
  remove_line(doc, y);
  return REMOVE_LINE;
}

void remove_range(struct Document *doc, int ay, int ax, int by, int bx) {
  get_selected_coordinates(&ay, &ax, &by, &bx);
  if (ay >= doc->len || by >= doc->len) return;
  struct Line *first = doc->buf[ay], *last = doc->buf[by];
  if (ax > first->len || bx > last->len) return;
  int remainder = last->len - bx - 1;
  if (remainder < 0) remainder = 0;
  int len = ax + remainder;
  if (first->size - 1 < len) {
    first->size = len + ADDITIONAL_REALLOCATION;
    first->buf = (char *)xrealloc(first->buf, first->size);
  }
  if (remainder) memcpy(first->buf + ax, last->buf + bx + 1, remainder);
  first->len = len;
  first->buf[len] = '\0';
  int lines_to_remove = by - ay;
  for (int i = 0; i < lines_to_remove; i++) {
    remove_line(doc, ay + 1);
  }
}

void line_break(struct Document *doc) {
  add_line(doc, NULL, doc->y + 1);
  struct Line *line = doc->buf[doc->y];
  struct Line *next = doc->buf[doc->y + 1];
  int diff = line->len - doc->x;
  if (diff > 0) {
    next->size = diff + 1;
    next->buf = (char *)xrealloc(next->buf, next->size);
    next->len = diff;
    memcpy(next->buf, line->buf + doc->x, diff);
    line->len = doc->x;
    line->buf[line->len] = next->buf[next->len] = '\0';
  }
}
