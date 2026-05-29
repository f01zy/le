#include <stdio.h>
#include <unistd.h>

#include "defines.h"
#include "render.h"
#include "service.h"

void render_line(struct Context *ctx, int y) {
  move_cursor_yx(y, 0);
  ANSI_RESET_LINE;
  struct Line *line = ctx->buf[y];
  write(STDOUT_FILENO, line->buf, line->len);
}

void render_statusline(struct Context *ctx) {
  char buf[MAX_BUFFER_SIZE];
  char *mode_label;
  if (ctx->mode == MODE_NORMAL) mode_label = "NORMAL";
  if (ctx->mode == MODE_INSERT) mode_label = "INSERT";
  int len = snprintf(buf, sizeof(buf), "--%s--", mode_label);
  move_cursor_yx(ctx->win.ws_row - 1, 0);
  write(STDOUT_FILENO, buf, len);
  move_cursor_yx(ctx->y, ctx->x);
}
