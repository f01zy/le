#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "defines.h"
#include "render.h"

void render_line(struct Context *ctx, struct Cell *buf, size_t len, int y) {
  move_cursor_yx(y, 0);
  ANSI_RESET_LINE_FROM_CURSOR;
  enum RenderMode mode = buf[0].mode;
  enum ForegroundColor fg = buf[0].fg;
  enum BackgroundColor bg = buf[0].bg;
  char temp[MAX_BUFFER_SIZE];
  int curr = 0;
  for (int i = 0; i < len; i++) {
    if (buf[i].mode != mode || buf[i].fg != fg || buf[i].bg != bg || i == len - 1) {
      set_render_mode(mode, fg, bg);
      write(STDOUT_FILENO, temp, curr);
      mode = buf[i].mode;
      fg = buf[i].fg;
      bg = buf[i].bg;
      curr = 0;
    }
    temp[curr++] = buf[i].ch;
  }
  if (curr > 0) {
    set_render_mode(mode, fg, bg);
    write(STDOUT_FILENO, temp, curr);
  }
}

void render_tabmenu(struct Context *ctx, struct Cell **frame) {
  for (int i = 0; i < ctx->win.ws_col; i++) {
    frame[0][i] = CELL(' ');
  }
  int max_len = 0;
  for (int i = 0; i < ctx->len; i++) {
    const char *filename = get_file_name(ctx->docs[i]->path);
    max_len = MAX(max_len, strlen(filename));
  }
  int max_docs_count = MAX(ctx->win.ws_col / max_len, 1);
  int offset = MAX((int)ctx->curr_doc - max_docs_count + 1, 0);
  for (int i = offset; i < offset + max_docs_count && i < ctx->len; i++) {
    enum RenderMode mode = i == ctx->curr_doc ? RENDER_DEFAULT : RENDER_DIM;
    const char *filename = get_file_name(ctx->docs[i]->path);
    int len = strlen(filename);
    int margin = (max_len - len) / 2;
    int tab_start_x = (i - offset) * max_len;
    for (int j = 0; j < len; j++) {
      frame[0][tab_start_x + j + margin] = CELL_MODE(filename[j], mode);
    }
  }
}

void render_line_numbers(struct Context *ctx, struct Document *doc, struct Cell **frame) {
  int width = get_line_number_margin(ctx), height = get_buffer_height(ctx);
  int offsetY = get_tabmenu_margin(ctx);
  char buf[MAX_BUFFER_SIZE];
  for (int i = 0; i < height; i++) {
    int y = doc->offsetY + i;
    if (y >= doc->len) {
      for (int j = 0; j < width; j++) {
        frame[i + offsetY][j] = CELL(' ');
      }
      continue;
    };
    enum RenderMode mode = doc->y == y ? RENDER_DEFAULT : RENDER_DIM;
    int len = snprintf(buf, sizeof(buf), "%d", y + 1);
    for (int j = 0; j < len; j++) {
      frame[i + offsetY][j] = CELL_MODE(buf[j], mode);
    }
    for (int j = len; j < width; j++) {
      frame[i + offsetY][j] = CELL(' ');
    }
  }
}

void render_statusline(struct Context *ctx, struct Document *doc, struct Cell **frame) {
  char buf[MAX_BUFFER_SIZE];
  int len = 0, y = ctx->win.ws_row - 1;
  enum ForegroundColor fg = FOREGROUND_DEFAULT;

  switch (ctx->status.mode) {
  case STATUS_MODE_MESSAGE:
    if (ctx->status.msg.level == MESSAGE_ERROR) fg = FOREGROUND_RED;
    if (ctx->status.msg.level == MESSAGE_WARNING) fg = FOREGROUND_YELLOW;
    len = snprintf(buf, sizeof(buf), "%s", ctx->status.msg.buf);
    break;

  case STATUS_MODE_NORMAL:;
    const char *label = ctx->mode == EDITOR_MODE_INSERT ? "INSERT" : "NORMAL";
    len = snprintf(buf, sizeof(buf), "-- %s -- %d/%d", label, doc->y + 1, doc->x + 1);
    break;

  case STATUS_MODE_COMMAND:
    len = snprintf(buf, sizeof(buf), ":%s", ctx->status.cmd.buf);
    break;

  case STATUS_MODE_DIALOG:
    len = snprintf(buf, sizeof(buf), "%s", ctx->status.dialog.buf);
    break;
  }

  for (int i = 0; i < ctx->win.ws_col; i++) {
    frame[y][i] = i < len ? (struct Cell){buf[i], RENDER_DEFAULT, fg, BACKGROUND_DEFAULT} : CELL(' ');
  }
}

void render_buf(struct Context *ctx, struct Document *doc, struct Cell **frame) {
  int offsetX = get_line_number_margin(ctx), offsetY = get_tabmenu_margin(ctx);
  int width = get_buffer_width(ctx), height = get_buffer_height(ctx);

  for (int i = 0; i < height; i++) {
    int y = i + doc->offsetY;
    if (y >= doc->len) {
      for (int j = 0; j < width; j++) {
        frame[i + offsetY][j + offsetX] = CELL(' ');
      }
      continue;
    }

    struct Line *line = doc->buf[y];
    for (int j = 0; j < width; j++) {
      int x = doc->offsetX + j;
      char ch = x < line->len ? line->buf[x] : ' ';
      frame[i + offsetY][j + offsetX] = CELL(ch);
    }

    // TODO: сделать какую-нибудь мемоизацию
    if (ctx->ui.is_code_highlighting && doc->tokens.buf) {
      struct TokenLine *tokens_line = doc->tokens.buf[y];
      for (int j = 0; j < tokens_line->len; j++) {
        struct Token *token = &tokens_line->buf[j];
        enum ForegroundColor fg = get_token_foreground(token->group);
        for (int k = token->start; k < token->start + token->len; k++) {
          int x = k - doc->offsetX;
          if (x < 0) continue;
          frame[i + offsetY][x + offsetX].fg = fg;
        }
      }
    }
  }
}

void render_mappings_menu(struct Context *ctx, struct Document *doc, struct Cell **frame) {
  struct MappingNode *node = ctx->curr_mapping;
  int offsetY = get_tabmenu_margin(ctx);
  int startY = ctx->win.ws_row - offsetY - MAPPINGS_COL;
  if (startY < 1) return;
  for (int i = startY - 1; i < startY + MAPPINGS_COL; i++) {
    for (int j = 0; j < ctx->win.ws_col; j++) {
      frame[i][j] = (struct Cell){' ', RENDER_DEFAULT, FOREGROUND_DEFAULT, BACKGROUND_BLACK};
    }
  }
  for (int i = 0; i < node->len; i++) {
    struct MappingNode *curr = node->nodes[i];
    char buf[MAX_BUFFER_SIZE];
    int len;
    if (curr->desc) {
      len = snprintf(buf, sizeof(buf), "%c - %s", curr->ch, curr->desc);
    } else {
      len = snprintf(buf, sizeof(buf), "%c +%zu mappings", curr->ch, curr->len);
    }
    int offsetX = (i / MAPPINGS_COL) * MAPPINGS_COL_WIDTH + 2;
    for (int j = 0; j < len && j < MAPPINGS_COL_WIDTH; j++) {
      int x = offsetX + j;
      int y = startY + i % MAPPINGS_COL;
      if (x >= ctx->win.ws_col) break;
      frame[y][x].ch = buf[j];
    }
  }
}

void render(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  render_buf(ctx, doc, ctx->curr_frame);
  if (ctx->ui.is_tabmenu) render_tabmenu(ctx, ctx->curr_frame);
  if (ctx->ui.is_line_numbers) render_line_numbers(ctx, doc, ctx->curr_frame);
  if (ctx->ui.is_statusline) render_statusline(ctx, doc, ctx->curr_frame);
  if (ctx->ui.is_mappings_menu) render_mappings_menu(ctx, doc, ctx->curr_frame);

  for (int i = 0; i < ctx->win.ws_row; i++) {
    for (int j = 0; j < ctx->win.ws_col; j++) {
      if (!ctx->prev_frame || memcmp(&ctx->curr_frame[i][j], &ctx->prev_frame[i][j], sizeof(ctx->curr_frame[i][j]))) {
        render_line(ctx, ctx->curr_frame[i], ctx->win.ws_col, i);
        break;
      }
    }
  }
  struct Cell **temp = ctx->prev_frame;
  ctx->prev_frame = ctx->curr_frame;
  ctx->curr_frame = temp;

  if (ctx->mode == EDITOR_MODE_COMMAND) {
    move_cursor_yx(ctx->win.ws_row - 1, ctx->status.cmd.len + 1);
  } else if (ctx->mode == EDITOR_MODE_DIALOG) {
    move_cursor_yx(ctx->win.ws_row - 1, strlen(ctx->status.dialog.buf));
  } else {
    int offsetX = get_line_number_margin(ctx);
    int offsetY = get_tabmenu_margin(ctx);
    move_cursor_yx(doc->y - doc->offsetY + offsetY, doc->x - doc->offsetX + offsetX);
  }
}
