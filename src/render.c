#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "frame.h"
#include "path.h"
#include "render.h"
#include "selected.h"
#include "terminal.h"
#include "types.h"
#include "ui.h"

void render_line(struct Cell *buf, size_t len, int y) {
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
  struct Vec2 size = get_tabmenu_size(ctx->ui, ctx->terminal.size);
  struct Vec2 offset = get_tabmenu_offset(ctx->ui, ctx->terminal.size);
  for (int i = 0; i < size.x; i++) {
    frame[offset.y][i + offset.x] = CELL(' ');
  }
  int max_len = 0;
  for (int i = 0; i < ctx->len; i++) {
    const char *filename = get_file_name(ctx->docs[i]->path);
    max_len = MAX(max_len, strlen(filename));
  }
  int max_docs_count = MAX(size.x / max_len, 1);
  int doc_offset = MAX((int)ctx->curr_doc - max_docs_count + 1, 0);
  for (int i = doc_offset; i < doc_offset + max_docs_count && i < ctx->len; i++) {
    enum RenderMode mode = i == ctx->curr_doc ? RENDER_DEFAULT : RENDER_DIM;
    const char *filename = get_file_name(ctx->docs[i]->path);
    size_t len = strlen(filename);
    int margin = (max_len - len) / 2;
    int tab_start_x = (i - doc_offset) * max_len;
    for (int j = 0; j < len; j++) {
      frame[offset.y][tab_start_x + j + margin + offset.x] = CELL_MODE(filename[j], mode);
    }
  }
}

void render_line_numbers(struct Context *ctx, struct Cell **frame) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  struct Vec2 size = get_line_numbers_size(ctx->ui, ctx->terminal.size, doc->len);
  struct Vec2 offset = get_line_numbers_offset(ctx->ui, ctx->terminal.size, doc->len);
  char buf[MAX_BUFFER_SIZE];
  for (int i = 0; i < size.y; i++) {
    int y = doc->offset.y + i;
    if (y >= doc->len) {
      for (int j = 0; j < size.x; j++) {
        frame[i + offset.y][j + offset.x] = CELL(j ? ' ' : '~');
      }
      continue;
    };
    enum RenderMode mode = doc->pos.y == y ? RENDER_DEFAULT : RENDER_DIM;
    int num = (ctx->ui.is_relative_line_numbers && doc->pos.y != y) ? abs(doc->pos.y - y) : y + 1;
    size_t len = snprintf(buf, sizeof(buf), "%d", num);
    for (int j = 0; j < len; j++) {
      frame[i + offset.y][j + offset.x] = CELL_MODE(buf[j], mode);
    }
    for (int j = len; j < size.x; j++) {
      frame[i + offset.y][j + offset.x] = CELL(' ');
    }
  }
}

void render_statusline(struct Context *ctx, struct Cell **frame) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  char buf[MAX_BUFFER_SIZE];
  size_t len = 0;
  int col = ctx->terminal.size.x, row = ctx->terminal.size.y;
  int y = row - 1;
  enum ForegroundColor fg = FOREGROUND_WHITE;

  switch (ctx->status.mode) {
  case STATUS_MODE_MESSAGE:
    if (ctx->status.msg.level == MESSAGE_ERROR) fg = FOREGROUND_RED;
    len = snprintf(buf, sizeof(buf), "%s", ctx->status.msg.buf);
    break;
  case STATUS_MODE_NORMAL:;
    const char *label = get_editor_mode_label(ctx->mode);
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
    frame[y][i] = i < len ? (struct Cell){buf[i], RENDER_DEFAULT, fg, BACKGROUND_DEFAULT} : CELL(' ');
  }
}

void render_file_tree(struct Context *ctx, struct Cell **frame) {
  struct Vec2 size = get_file_tree_size(ctx->ui, ctx->terminal.size);
  struct Vec2 offset = get_file_tree_offset(ctx->ui, ctx->terminal.size);
  for (int i = 0; i < size.y; i++) {
    int y = i + ctx->file_tree.offset;
    if (y >= ctx->file_tree.len) {
      for (int j = 0; j < size.x; j++) {
        frame[i + offset.y][j + offset.x] = CELL(' ');
      }
      continue;
    }
    size_t level = ctx->file_tree.labels[y].level;
    struct FileTreeEntity *ent = ctx->file_tree.labels[y].ent;
    const char *ent_name = get_file_name(ent->path);
    size_t name_len = strlen(ent_name);
    enum BackgroundColor bg = ctx->file_tree.pos == y ? BACKGROUND_GRAY : BACKGROUND_DEFAULT;
    for (int j = 0; j < level; j++) {
      frame[i + offset.y][j + offset.x] = (struct Cell){' ', RENDER_DEFAULT, FOREGROUND_WHITE, bg};
    }
    for (int j = level; j < name_len + level && j < size.x; j++) {
      frame[i + offset.y][j + offset.x] = (struct Cell){ent_name[j - level], RENDER_DEFAULT, FOREGROUND_WHITE, bg};
    }
    for (int j = level + name_len; j < size.x; j++) {
      frame[i + offset.y][j + offset.x] = (struct Cell){' ', RENDER_DEFAULT, FOREGROUND_WHITE, bg};
    }
  }
}

void render_buf_highlights(struct Context *ctx, struct Cell **frame) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  struct Vec2 size = get_buf_size(ctx->ui, ctx->terminal.size, doc->len);
  struct Vec2 offset = get_buf_offset(ctx->ui, ctx->terminal.size, doc->len);
  for (int i = 0; i < size.y; i++) {
    int y = i + doc->offset.y;
    if (y >= doc->len) break;
    struct TokenLine *line = doc->tokens.buf[y];
    for (int j = 0; j < line->len; j++) {
      struct Token *token = &line->buf[j];
      enum ForegroundColor fg = get_token_foreground(token->group);
      for (int k = token->start; k < token->start + token->len; k++) {
        int x = k - doc->offset.x;
        if (x < 0) continue;
        frame[i + offset.y][x + offset.x].fg = fg;
      }
    }
  }
}

void render_buf(struct Context *ctx, struct Cell **frame) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  struct Vec2 size = get_buf_size(ctx->ui, ctx->terminal.size, doc->len);
  struct Vec2 offset = get_buf_offset(ctx->ui, ctx->terminal.size, doc->len);
  for (int i = 0; i < size.y; i++) {
    int y = i + doc->offset.y;
    if (y >= doc->len) {
      for (int j = 0; j < size.x; j++) {
        frame[i + offset.y][j + offset.x] = CELL(' ');
      }
      continue;
    }
    struct Line *line = doc->buf[y];
    for (int j = 0; j < size.x; j++) {
      int x = doc->offset.x + j;
      char ch = x < line->len ? line->buf[x] : ' ';
      enum BackgroundColor bg = BACKGROUND_DEFAULT;
      if (ctx->mode == EDITOR_MODE_VISUAL) {
        struct Vec4 c = {doc->pos.x, doc->pos.y, doc->selected.x, doc->selected.y};
        get_selected_coordinates(&c);
        if (is_within_range(c, (struct Vec2){x, y})) bg = BACKGROUND_GRAY;
      }
      frame[i + offset.y][j + offset.x] = (struct Cell){ch, RENDER_DEFAULT, FOREGROUND_WHITE, bg};
    }
  }
  if (ctx->ui.is_code_highlighting && doc->tokens.buf) render_buf_highlights(ctx, frame);
}

void render(struct Context *ctx) {
  struct Cell **prev_frame = ctx->frame.prev, **curr_frame = ctx->frame.curr;
  struct Document *doc = ctx->docs[ctx->curr_doc];
  int col = ctx->terminal.size.x, row = ctx->terminal.size.y;
  render_buf(ctx, curr_frame);

  if (ctx->ui.is_tabmenu) render_tabmenu(ctx, curr_frame);
  if (ctx->ui.is_statusline) render_statusline(ctx, curr_frame);
  if (ctx->ui.is_file_tree) render_file_tree(ctx, curr_frame);
  if (ctx->ui.is_line_numbers) render_line_numbers(ctx, curr_frame);

  ANSI_HIDE_CURSOR;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      if (!prev_frame || memcmp(&curr_frame[i][j], &prev_frame[i][j], sizeof(curr_frame[i][j]))) {
        render_line(curr_frame[i], col, i);
        break;
      }
    }
  }
  ANSI_SHOW_CURSOR;
  swap_frames(ctx);

  if (ctx->mode == EDITOR_MODE_COMMAND) {
    move_cursor_yx(row - 1, ctx->status.cmd.len + 1);
  } else if (ctx->mode == EDITOR_MODE_DIALOG) {
    move_cursor_yx(row - 1, strlen(ctx->status.dialog.buf));
  } else if (ctx->focus == EDITOR_FOCUS_BUFFER) {
    struct Vec2 offset = get_buf_offset(ctx->ui, ctx->terminal.size, doc->len);
    move_cursor_yx(doc->pos.y - doc->offset.y + offset.y, doc->pos.x - doc->offset.x + offset.x);
  } else if (ctx->focus == EDITOR_FOCUS_TREE) {
    struct Vec2 offset = get_file_tree_offset(ctx->ui, ctx->terminal.size);
    move_cursor_yx(ctx->file_tree.pos - ctx->file_tree.offset + offset.y, offset.x);
  }
}
