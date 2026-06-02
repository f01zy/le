#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"

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
  int len;
  if (path[0] == '/') {
    len = snprintf(buf, sizeof(buf), "%s", path);
  } else {
    len = snprintf(buf, sizeof(buf), "%s/%s", cwd, path);
  }
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
  doc->len++;
  if (doc->len > doc->size) {
    doc->size = doc->len + ADDITIONAL_REALLOCATION;
    doc->buf = (struct Line **)xrealloc(doc->buf, doc->size * sizeof(struct Line *));
  }
  struct Line *line = (struct Line *)xmalloc(sizeof(struct Line));
  if (data) {
    int len = strlen(data);
    line->buf = (char *)xmalloc(len + 1);
    line->len = len;
    line->size = len + 1;
    memcpy(line->buf, data, len);
  } else {
    line->buf = (char *)xmalloc(1);
    line->len = 0;
    line->size = 1;
  }
  line->buf[line->len] = '\0';
  for (int i = doc->len - 1; i > y; i--) {
    doc->buf[i] = doc->buf[i - 1];
  }
  doc->buf[y] = line;
}

void remove_line(struct Document *doc, int y) {
  if (y >= doc->len) return;
  struct Line *line = doc->buf[y];
  free(line->buf);
  free(line);
  doc->len--;
  for (int i = y; i < doc->len; i++) {
    doc->buf[i] = doc->buf[i + 1];
  }
}

void write_to_line(struct Document *doc, int y, int x, char ch) {
  if (y >= doc->len) return;
  struct Line *line = doc->buf[y];
  if (x > line->len) return;
  line->len++;
  if (line->len + 1 > line->size) {
    line->size = line->len + ADDITIONAL_REALLOCATION;
    line->buf = (char *)xrealloc(line->buf, line->size);
  }
  for (int i = line->len - 1; i > x; i--) {
    line->buf[i] = line->buf[i - 1];
  }
  line->buf[x] = ch;
  line->buf[line->len] = '\0';
}

enum RemoveResult remove_from_line(struct Document *doc, int y, int x) {
  if (y >= doc->len) return REMOVE_NOTHING;
  struct Line *line = doc->buf[y];
  if (x > line->len || (!x && !y)) return REMOVE_NOTHING;
  if (x) {
    line->len--;
    for (int i = x - 1; i < line->len; i++) {
      line->buf[i] = line->buf[i + 1];
    }
    line->buf[line->len] = '\0';
    return REMOVE_CHAR;
  }
  struct Line *prev = doc->buf[y - 1];
  if (line->len > 0) {
    int len = prev->len + line->len;
    if (prev->size - prev->len - 1 < line->len) {
      prev->size = len;
      prev->buf = (char *)xrealloc(prev->buf, prev->size);
    }
    memcpy(prev->buf + prev->len, line->buf, line->len);
    prev->len = len;
    prev->buf[prev->len] = '\0';
  }
  remove_line(doc, y);
  return REMOVE_LINE;
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
    line->buf[line->len] = '\0';
    next->buf[next->len] = '\0';
  }
}
