#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "defines.h"
#include "service.h"
#include "types.h"

void configure_context(struct Context *ctx) {
  ioctl(STDIN_FILENO, TIOCGWINSZ, ctx->size);
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

void move_cursor_yx(int y, int x) {
  char buf[MAX_BUFFER_SIZE];
  int len = snprintf(buf, sizeof(buf), ANSI_MOVE_CURSOR_YX, y + 1, x + 1);
  write(STDOUT_FILENO, buf, len);
}

void add_line(struct Context *ctx, int y) {
  ctx->len++;
  if (ctx->len > ctx->size) {
    ctx->size = ctx->len + 8;
    ctx->buf  = (struct Line **)realloc(ctx->buf, ctx->size * sizeof(struct Line *));
  }
  struct Line *line = (struct Line *)malloc(sizeof(struct Line));
  line->buf         = malloc(1);
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
    line->size = line->len + 32;
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
  for (int i = ctx->x; i < line->len; i++) {
    write_to_line(ctx, ctx->y + 1, next->len, line->buf[i]);
    remove_from_line(ctx, ctx->y, i);
  }
}

void render_line(struct Context *ctx, int y) {
  move_cursor_yx(y, 0);
  ANSI_RESET_LINE;
  struct Line *line = ctx->buf[y];
  write(STDOUT_FILENO, line->buf, line->len);
}
