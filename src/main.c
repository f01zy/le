#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "defines.h"
#include "render.h"
#include "service.h"
#include "types.h"

struct Context ctx;

void handle_normal_mode(struct Context *ctx, int ch) {
  struct Line *line = ctx->buf[ctx->y];
  size_t len        = get_max_x(line);

  switch (ch) {
  case 'h':
    if (ctx->x == 0 && ctx->y == 0) break;
    if (ctx->x == 0) {
      ctx->x = get_max_x(ctx->buf[ctx->y - 1]);
      ctx->y--;
    } else {
      ctx->x--;
    }
    break;

  case 'l':
    if (ctx->x == len && ctx->y == ctx->len - 1) break;
    if (ctx->x == len) {
      ctx->x = 0;
      ctx->y++;
    } else {
      ctx->x++;
    }
    break;

  case 'j':
    if (ctx->y == ctx->len - 1) break;
    ctx->y++;
    ctx->x = MIN(ctx->x, ctx->buf[ctx->y]->len);
    break;

  case 'k':
    if (ctx->y == 0) break;
    ctx->y--;
    ctx->x = MIN(ctx->x, ctx->buf[ctx->y]->len);
    break;

  case '$':
    ctx->x = len;
    break;

  case '0':
    ctx->x = 0;
    break;

  case 'i':
    change_mode(ctx, MODE_INSERT);
    break;

  case 'a':
    if (line->len > 0) ctx->x++;
    change_mode(ctx, MODE_INSERT);
    break;

  case ':':
    change_mode(ctx, MODE_COMMAND);
    break;
  }
}

void handle_insert_mode(struct Context *ctx, int ch) {
  switch (ch) {
  case KEY_ENTER:
    line_break(ctx);
    ctx->y++;
    ctx->x = 0;
    break;

  case KEY_BACKSPACE:;
    int temp              = ctx->y > 0 ? ctx->buf[ctx->y - 1]->len : 0;
    enum RemoveResult res = remove_from_line(ctx, ctx->y, ctx->x);
    if (res == REMOVE_CHAR) {
      ctx->x--;
    } else if (res == REMOVE_LINE) {
      ctx->x = temp;
      ctx->y--;
    }
    break;

  case KEY_ESCAPE:
    if (ctx->x) ctx->x--;
    change_mode(ctx, MODE_NORMAL);
    break;

  default:
    if (ch >= 32 && ch <= 126) {
      write_to_line(ctx, ctx->y, ctx->x, ch);
      ctx->x++;
    }
    break;
  }
}

void handle_command(struct Context *ctx) {
  if (strcmp(ctx->cmd->buf, "quit") == 0) { ctx->is_exit = true; }
}

void handle_command_mode(struct Context *ctx, char ch) {
  switch (ch) {
  case KEY_ESCAPE:
    clear_cmd(ctx);
    change_mode(ctx, MODE_NORMAL);
    break;

  case KEY_ENTER:
    handle_command(ctx);
    clear_cmd(ctx);
    change_mode(ctx, MODE_NORMAL);
    break;

  case KEY_BACKSPACE:
    if (ctx->cmd->len > 0) ctx->cmd->buf[--ctx->cmd->len] = '\0';
    break;

  default:
    if (ch >= 32 && ch <= 126 && ctx->cmd->len < ctx->cmd->size - 1) {
      ctx->cmd->buf[ctx->cmd->len++] = ch;
      ctx->cmd->buf[ctx->cmd->len]   = '\0';
    }
    break;
  }
}

int main() {
  ANSI_RESET_SCREEN;
  configure_context(&ctx);
  change_mode(&ctx, MODE_NORMAL);
  render(&ctx);

  int ch;
  while (!ctx.is_exit) {
    ch = getchar();
    if (ctx.mode == MODE_NORMAL) {
      handle_normal_mode(&ctx, ch);
    } else if (ctx.mode == MODE_INSERT) {
      handle_insert_mode(&ctx, ch);
    } else if (ctx.mode == MODE_COMMAND) {
      handle_command_mode(&ctx, ch);
    }
    check_offset(&ctx);
    render(&ctx);
  }

  free_resources(&ctx);
  ANSI_RESET_SCREEN;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &ctx.backup);
  return 0;
}
