#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#include "defines.h"
#include "service.h"
#include "types.h"

struct Context ctx;

void handle_normal_mode(struct Context *ctx, int ch) {
  if (ch == 'h') {
    if (ctx->x == 0 && ctx->y == 0) return;
    if (ctx->x == 0) {
      ctx->y--;
      struct Line *line = ctx->buf[ctx->y];
      ctx->x            = line->len;
    } else {
      ctx->x--;
    }
  }

  else if (ch == 'l') {
    struct Line *line = ctx->buf[ctx->y];
    if (ctx->x == line->len && ctx->y == ctx->len - 1) return;
    if (ctx->x == line->len) {
      ctx->x = 0;
      ctx->y++;
    } else {
      ctx->x++;
    }
  }

  else if (ch == 'j') {
    if (ctx->y == ctx->len - 1) return;
    ctx->y++;
    struct Line *line = ctx->buf[ctx->y];
    ctx->x            = MIN(ctx->x, line->len);
  }

  else if (ch == 'k') {
    if (ctx->y == 0) return;
    ctx->y--;
    struct Line *line = ctx->buf[ctx->y];
    ctx->x            = MIN(ctx->x, line->len);
  }

  else if (ch == 'i') {
    ctx->mode = MODE_INSERT;
  }
  move_cursor_yx(ctx->y, ctx->x);
}

void handle_insert_mode(struct Context *ctx, int ch) {
  if (ch == KEY_ESCAPE) {
    ctx->mode = MODE_NORMAL;
    return;
  }

  if (ch == KEY_ENTER) {
    line_break(ctx);
    render_line(ctx, ctx->y);
    render_line(ctx, ctx->y + 1);
    ctx->y++;
    ctx->x = 0;
  }

  else if (ch == KEY_BACKSPACE) {
    enum RemoveResult res = remove_from_line(ctx, ctx->y, ctx->x);
    if (res == REMOVE_CHAR) {
      ctx->x--;
      render_line(ctx, ctx->y);
    } else if (res == REMOVE_LINE) {
      render_line(ctx, ctx->y - 1);
      render_line(ctx, ctx->y);
      ctx->x = ctx->buf[ctx->y - 1]->len;
      ctx->y--;
    }
  }

  else if (ch >= 32 && ch <= 126) {
    write_to_line(ctx, ctx->y, ctx->x, ch);
    ctx->x++;
    render_line(ctx, ctx->y);
  }
  move_cursor_yx(ctx->y, ctx->x);
}

int main() {
  configure_context(&ctx);
  move_cursor_yx(0, 0);
  ANSI_RESET_SCREEN;

  int ch;
  while ((ch = getchar()) != KEY_TAB) {
    if (ctx.mode == MODE_NORMAL) {
      handle_normal_mode(&ctx, ch);
    } else if (ctx.mode == MODE_INSERT) {
      handle_insert_mode(&ctx, ch);
    }
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &ctx.backup);
  return 0;
}
