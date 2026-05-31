#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "handlers.h"

void handle_normal_mode(struct Context *ctx, int ch) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  struct Line *line = doc->buf[doc->y];
  size_t len = get_max_x(line);

  switch (ch) {
  case 'h':
    if (!doc->x && !doc->y) break;
    if (!doc->x) {
      doc->x = get_max_x(doc->buf[doc->y - 1]);
      doc->y--;
    } else {
      doc->x--;
    }
    break;

  case 'l':
    if (doc->x == len && doc->y == doc->len - 1) break;
    if (doc->x == len) {
      doc->x = 0;
      doc->y++;
    } else {
      doc->x++;
    }
    break;

  case 'j':
    if (doc->y == doc->len - 1) break;
    doc->x = MIN(doc->x, get_max_x(doc->buf[doc->y + 1]));
    doc->y++;
    break;

  case 'k':
    if (!doc->y) break;
    doc->x = MIN(doc->x, get_max_x(doc->buf[doc->y - 1]));
    doc->y--;
    break;

  case '$':
    doc->x = len;
    break;

  case '0':
    doc->x = 0;
    break;

  case 'i':
    change_mode(ctx, MODE_INSERT);
    break;

  case 'a':
    if (line->len > 0) doc->x++;
    change_mode(ctx, MODE_INSERT);
    break;

  case ':':
    change_mode(ctx, MODE_COMMAND);
    break;
  }
}

void handle_insert_mode(struct Context *ctx, int ch) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  switch (ch) {
  case KEY_ENTER:
    line_break(doc);
    doc->y++;
    doc->x = 0;
    break;

  case KEY_BACKSPACE:;
    int temp = doc->y > 0 ? doc->buf[doc->y - 1]->len : 0;
    enum RemoveResult res = remove_from_line(doc, doc->y, doc->x);
    if (res == REMOVE_CHAR) {
      doc->x--;
    } else if (res == REMOVE_LINE) {
      doc->x = temp;
      doc->y--;
    }
    break;

  case KEY_ESCAPE:
    if (doc->x) doc->x--;
    change_mode(ctx, MODE_NORMAL);
    break;

  default:
    if (ch >= 32 && ch <= 126) {
      write_to_line(doc, doc->y, doc->x, ch);
      doc->x++;
    }
    break;
  }
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
      ctx->cmd->buf[ctx->cmd->len] = '\0';
    }
    break;
  }
}
