#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "filesystem.h"
#include "memory.h"
#include "path.h"
#include "selected.h"

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

bool init_doc(struct Document *doc, const char *path) {
  struct GetDocDataRes res = get_doc_data(path);
  if (!res.data) return false;
  set_doc_path(doc, path);
  // TODO: Если до этого был открыт другой файл, надо очистить текущий буфер
  doc->buf = res.data;
  doc->len = res.len;
  doc->offset.x = doc->offset.y = 0;
  doc->pos.x = doc->pos.y = 0;
  return true;
}

size_t save_doc(struct Document *doc) { return save_doc_data(doc->path, doc->buf, doc->len); }

void set_doc_path(struct Document *doc, const char *path) {
  if (!path) return;
  free(doc->path);
  char cwd[MAX_BUFFER_SIZE];
  if (!get_curr_dir(cwd, sizeof(cwd))) return;
  char buf[MAX_BUFFER_SIZE];

  bool is_absolute_path = false;
#ifdef WIN32
  if (strlen(path) >= 3 && isalpha(path[0]) && path[1] == ':' && path[2] == PATH_SEPARATOR) is_absolute_path = true;
#else
  if (path[0] == PATH_SEPARATOR) is_absolute_path = true;
#endif

  size_t len;
  if (!is_absolute_path) {
    char dirty[MAX_BUFFER_SIZE];
    len = snprintf(dirty, sizeof(dirty), "%s%c%s", cwd, PATH_SEPARATOR, path);
    get_real_path(dirty, len, buf, sizeof(buf));
  } else {
    len = snprintf(buf, sizeof(buf), "%s", path);
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
  if (doc->len + 1 > doc->size) {
    doc->size = doc->len + 1 + ADDITIONAL_REALLOCATION;
    doc->buf = (struct Line **)xrealloc(doc->buf, doc->size * sizeof(struct Line *));
  }
  memmove(&doc->buf[y + 1], &doc->buf[y], (doc->len - y) * sizeof(struct Line *));
  struct Line *line = (struct Line *)xmalloc(sizeof(struct Line));
  size_t len = data ? strlen(data) : 0;
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
    size_t len = prev->len + line->len;
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

void remove_range(struct Document *doc, struct Vec4 c) {
  get_selected_coordinates(&c);
  if (c.ay >= doc->len || c.by >= doc->len) return;
  struct Line *first = doc->buf[c.ay], *last;
  size_t remainder;
  if (c.ay == c.by && c.ax == first->len && c.ay < doc->len - 1) {
    last = doc->buf[++c.by];
    remainder = last->len;
  } else {
    last = doc->buf[c.by];
    remainder = MAX((long long)last->len - c.bx - 1, 0);
  }
  if (c.ax > first->len || c.bx > last->len) return;
  size_t len = c.ax + remainder;
  if (first->size - 1 < len) {
    first->size = len + ADDITIONAL_REALLOCATION;
    first->buf = (char *)xrealloc(first->buf, first->size);
  }
  if (remainder) memcpy(first->buf + c.ax, last->buf + last->len - remainder, remainder);
  first->len = len;
  first->buf[len] = '\0';
  int lines_to_remove = c.by - c.ay;
  for (int i = 0; i < lines_to_remove; i++) {
    remove_line(doc, c.ay + 1);
  }
}

void line_break(struct Document *doc) {
  add_line(doc, NULL, doc->pos.y + 1);
  struct Line *line = doc->buf[doc->pos.y];
  struct Line *next = doc->buf[doc->pos.y + 1];
  int diff = line->len - doc->pos.x;
  if (diff > 0) {
    next->size = diff + 1;
    next->buf = (char *)xrealloc(next->buf, next->size);
    next->len = diff;
    memcpy(next->buf, line->buf + doc->pos.x, diff);
    line->len = doc->pos.x;
    line->buf[line->len] = next->buf[next->len] = '\0';
  }
}
