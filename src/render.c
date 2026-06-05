#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "render.h"

void render_line(struct Context *ctx, struct Cell *buf, size_t len, int y) {
  move_cursor_yx(y, 0);
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
  int col = ctx->terminal.win.ws_col, row = ctx->terminal.win.ws_row;
  for (int i = 0; i < col; i++) {
    frame[0][i] = CELL(' ');
  }
  int max_len = 0;
  for (int i = 0; i < ctx->len; i++) {
    const char *filename = get_file_name(ctx->docs[i]->path);
    max_len = MAX(max_len, strlen(filename));
  }
  int max_docs_count = MAX(col / max_len, 1);
  int offset = MAX((int)ctx->curr_doc - max_docs_count + 1, 0);
  for (int i = offset; i < offset + max_docs_count && i < ctx->len; i++) {
    enum RenderMode mode = i == ctx->curr_doc ? RENDER_DEFAULT : RENDER_DIM;
    const char *filename = get_file_name(ctx->docs[i]->path);
    size_t len = strlen(filename);
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
    int y = doc->offset.y + i;
    if (y >= doc->len) {
      for (int j = 0; j < width; j++) {
        frame[i + offsetY][j] = CELL(' ');
      }
      continue;
    };
    enum RenderMode mode = doc->pos.y == y ? RENDER_DEFAULT : RENDER_DIM;
    int num = (ctx->ui.is_relative_line_numbers && doc->pos.y != y) ? abs(doc->pos.y - y) : y + 1;
    size_t len = snprintf(buf, sizeof(buf), "%d", num);
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
  size_t len = 0;
  int col = ctx->terminal.win.ws_col, row = ctx->terminal.win.ws_row;
  int y = row - 1;
  enum ForegroundColor fg = FOREGROUND_WHITE;

  switch (ctx->status.mode) {
  case STATUS_MODE_MESSAGE:
    if (ctx->status.msg.level == MESSAGE_ERROR) fg = FOREGROUND_RED;
    if (ctx->status.msg.level == MESSAGE_WARNING) fg = FOREGROUND_YELLOW;
    len = snprintf(buf, sizeof(buf), "%s", ctx->status.msg.buf);
    break;

  case STATUS_MODE_NORMAL:;
    const char *label = get_editor_mode_label(ctx);
    len = snprintf(buf, sizeof(buf), "-- %s -- %d/%d %s", label, doc->pos.y + 1, doc->pos.x + 1, ctx->mapping.buf);
    break;

  case STATUS_MODE_COMMAND:
    len = snprintf(buf, sizeof(buf), ":%s", ctx->status.cmd.buf);
    break;

  case STATUS_MODE_DIALOG:
    len = snprintf(buf, sizeof(buf), "%s", ctx->status.dialog.buf);
    break;
  }

  for (int i = 0; i < col; i++) {
    frame[y][i] = i < len ? (struct Cell){buf[i], RENDER_DEFAULT, fg, BACKGROUND_BLACK} : CELL(' ');
  }
}

void render_buf(struct Context *ctx, struct Document *doc, struct Cell **frame) {
  int offsetX = get_line_number_margin(ctx), offsetY = get_tabmenu_margin(ctx);
  int width = get_buffer_width(ctx), height = get_buffer_height(ctx);

  for (int i = 0; i < height; i++) {
    int y = i + doc->offset.y;
    if (y >= doc->len) {
      for (int j = 0; j < width; j++) {
        frame[i + offsetY][j + offsetX] = CELL(' ');
      }
      continue;
    }

    struct Line *line = doc->buf[y];
    for (int j = 0; j < width; j++) {
      int x = doc->offset.x + j;
      char ch = x < line->len ? line->buf[x] : ' ';

      enum BackgroundColor bg = BACKGROUND_BLACK;
      if (ctx->mode == EDITOR_MODE_VISUAL) {
        struct Vec4 c = {doc->pos.x, doc->pos.y, doc->selected.x, doc->selected.y};
        get_selected_coordinates(&c);
        if ((y == c.ay && c.ay == c.by && x >= c.ax && x <= c.bx) ||
            (c.ay != c.by && ((y > c.ay && y < c.by) || (y == c.ay && x >= c.ax) || (y == c.by && x <= c.bx)))) {
          bg = BACKGROUND_GRAY;
        }
      }

      frame[i + offsetY][j + offsetX] = (struct Cell){ch, RENDER_DEFAULT, FOREGROUND_WHITE, bg};
    }

    if (!(ctx->ui.is_code_highlighting && doc->tokens.buf)) continue;
    struct TokenLine *tokens_line = doc->tokens.buf[y];
    for (int j = 0; j < tokens_line->len; j++) {
      struct Token *token = &tokens_line->buf[j];
      enum ForegroundColor fg = get_token_foreground(token->group);
      for (int k = token->start; k < token->start + token->len; k++) {
        int x = k - doc->offset.x;
        if (x < 0) continue;
        frame[i + offsetY][x + offsetX].fg = fg;
      }
    }
  }
}

// TODO: удалил curr_mapping, надо создать метод его поиска
void render_mappings_menu(struct Context *ctx, struct Document *doc, struct Cell **frame) {
  int col = ctx->terminal.win.ws_col, row = ctx->terminal.win.ws_row;
  struct MappingNode *node = ctx->mapping.head;
  int offsetY = get_tabmenu_margin(ctx);
  int startY = row - offsetY - MAPPINGS_COL;
  if (startY < 1) return;
  for (int i = startY - 1; i < startY + MAPPINGS_COL; i++) {
    for (int j = 0; j < col; j++) {
      frame[i][j] = (struct Cell){' ', RENDER_DEFAULT, FOREGROUND_WHITE, BACKGROUND_GRAY};
    }
  }
  for (int i = 0; i < node->len; i++) {
    struct MappingNode *curr = node->nodes[i];
    char buf[MAX_BUFFER_SIZE];
    size_t len = curr->desc ? snprintf(buf, sizeof(buf), "%c - %s", curr->ch, curr->desc) : snprintf(buf, sizeof(buf), "%c +%zu mappings", curr->ch, curr->len);
    int offsetX = (i / MAPPINGS_COL) * MAPPINGS_COL_WIDTH + 2;
    for (int j = 0; j < len && j < MAPPINGS_COL_WIDTH; j++) {
      int x = offsetX + j;
      int y = startY + i % MAPPINGS_COL;
      if (x >= col) break;
      frame[y][x].ch = buf[j];
    }
  }
}

void render(struct Context *ctx) {
  struct Cell **prev_frame = ctx->frame.prev, **curr_frame = ctx->frame.curr;
  struct Document *doc = ctx->docs[ctx->curr_doc];
  int col = ctx->terminal.win.ws_col, row = ctx->terminal.win.ws_row;
  render_buf(ctx, doc, curr_frame);

  if (ctx->ui.is_tabmenu) render_tabmenu(ctx, curr_frame);
  if (ctx->ui.is_line_numbers) render_line_numbers(ctx, doc, curr_frame);
  if (ctx->ui.is_statusline) render_statusline(ctx, doc, curr_frame);
  if (ctx->ui.is_mappings_menu) render_mappings_menu(ctx, doc, curr_frame);

  ANSI_HIDE_CURSOR;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      if (!prev_frame || memcmp(&curr_frame[i][j], &prev_frame[i][j], sizeof(curr_frame[i][j]))) {
        render_line(ctx, curr_frame[i], col, i);
        break;
      }
    }
  }
  ANSI_SHOW_CURSOR;

  ctx->frame.prev = curr_frame;
  ctx->frame.curr = prev_frame;
  if (ctx->mode == EDITOR_MODE_COMMAND) {
    move_cursor_yx(row - 1, ctx->status.cmd.len + 1);
  } else if (ctx->mode == EDITOR_MODE_DIALOG) {
    move_cursor_yx(row - 1, strlen(ctx->status.dialog.buf));
  } else {
    int offsetX = get_line_number_margin(ctx);
    int offsetY = get_tabmenu_margin(ctx);
    move_cursor_yx(doc->pos.y - doc->offset.y + offsetY, doc->pos.x - doc->offset.x + offsetX);
  }
}
