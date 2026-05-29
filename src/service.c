#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "defines.h"
#include "service.h"
#include "types.h"

void configure_context(struct Context *ctx) {
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ctx->win);
  tcgetattr(STDIN_FILENO, &ctx->backup);
  ctx->mode          = MODE_NORMAL;
  ctx->conf          = ctx->backup;
  ctx->conf.c_iflag |= IXOFF;
  ctx->conf.c_iflag &= ~ICRNL;
  ctx->conf.c_lflag &= ~ECHO;
  ctx->conf.c_lflag &= ~ISIG;
  ctx->conf.c_lflag &= ~ICANON;
  add_line(ctx, 0);
  tcsetattr(STDIN_FILENO, TCSANOW, &ctx->conf);
}

void change_mode(struct Context *ctx, enum Mode mode) {
  enum CursorStyle style;
  if (mode == MODE_NORMAL) style = CURSOR_BLOCK_STATIC;
  if (mode == MODE_INSERT) style = CURSOR_LINE_STATIC;
  ctx->mode = mode;
  set_cursor_type(style);
}

void move_cursor_yx(int y, int x) {
  char buf[MAX_BUFFER_SIZE];
  int len = snprintf(buf, sizeof(buf), ANSI_MOVE_CURSOR_YX, y + 1, x + 1);
  write(STDOUT_FILENO, buf, len);
}

void set_cursor_type(enum CursorStyle type) {
  char buf[MAX_BUFFER_SIZE];
  int len = snprintf(buf, sizeof(buf), ANSI_CURSOR_TYPE, type);
  write(STDOUT_FILENO, buf, len);
}

void add_line(struct Context *ctx, int y) {
  ctx->len++;
  if (ctx->len > ctx->size) {
    ctx->size = ctx->len + ADDITIONAL_REALLOCATION;
    ctx->buf  = (struct Line **)realloc(ctx->buf, ctx->size * sizeof(struct Line *));
  }
  struct Line *line = (struct Line *)malloc(sizeof(struct Line));
  line->buf         = (char *)malloc(1);
  line->buf[0]      = '\0';
  line->size        = 1;
  line->len         = 0;
  for (int i = ctx->len - 1; i > y; i--) {
    ctx->buf[i] = ctx->buf[i - 1];
  }
  ctx->buf[y] = line;
}

void remove_line(struct Context *ctx, int y) {
  struct Line *line = ctx->buf[y];
  ctx->len--;
  free(line->buf);
  free(line);
  for (int i = y; i < ctx->len; i++) {
    ctx->buf[i] = ctx->buf[i + 1];
  }
}

void write_to_line(struct Context *ctx, int y, int x, char ch) {
  struct Line *line = ctx->buf[y];
  line->len++;
  if (line->len + 1 > line->size) {
    line->size = line->len + ADDITIONAL_REALLOCATION;
    line->buf  = (char *)realloc(line->buf, line->size);
  }
  for (int i = line->len - 1; i > x; i--) {
    line->buf[i] = line->buf[i - 1];
  }
  line->buf[x]         = ch;
  line->buf[line->len] = '\0';
}

enum RemoveResult remove_from_line(struct Context *ctx, int y, int x) {
  struct Line *line = ctx->buf[y];
  if (x == 0) {
    if (y == 0) return REMOVE_NOTHING;
    struct Line *prev = ctx->buf[y - 1];
    if (line->len > 0) {
      int len  = prev->len + line->len;
      int diff = prev->size - prev->len - 1;
      if (diff < line->len) {
        prev->size = len;
        prev->buf  = (char *)realloc(prev->buf, prev->size);
      }
      memcpy(prev->buf + prev->len, line->buf, line->len);
      prev->len            = len;
      prev->buf[prev->len] = '\0';
    }
    remove_line(ctx, y);
    return REMOVE_LINE;
  }
  line->len--;
  for (int i = x - 1; i < line->len; i++) {
    line->buf[i] = line->buf[i + 1];
  }
  line->buf[line->len] = '\0';
  return REMOVE_CHAR;
}

void line_break(struct Context *ctx) {
  add_line(ctx, ctx->y + 1);
  struct Line *line = ctx->buf[ctx->y];
  struct Line *next = ctx->buf[ctx->y + 1];
  int diff          = line->len - ctx->x;
  if (diff > 0) {
    next->size = diff + 1;
    next->buf  = (char *)realloc(next->buf, next->size);
    next->len  = diff;
    memcpy(next->buf, line->buf + ctx->x, diff);
    line->len            = ctx->x;
    line->buf[line->len] = '\0';
    next->buf[next->len] = '\0';
  }
}
