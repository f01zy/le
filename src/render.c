#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "defines.h"
#include "render.h"
#include "service.h"
#include "types.h"

void render_line(struct Context *ctx, struct Cell *buf, size_t len, int y) {
  move_cursor_yx(y, 0);
  ANSI_RESET_LINE_FROM_CURSOR;
  enum RenderMode mode = buf[0].mode;
  char temp[MAX_BUFFER_SIZE];
  int curr = 0;
  for (int i = 0; i < len; i++) {
    if (buf[i].mode != mode || i == len - 1) {
      set_render_mode(mode);
      write(STDOUT_FILENO, temp, curr);
      mode = buf[i].mode;
      curr = 0;
    }
    temp[curr++] = buf[i].ch;
  }
}

void render_line_number(struct Context *ctx, struct Cell **frame) {
  int margin = get_line_number_margin(ctx);
  char buf[MAX_BUFFER_SIZE];
  for (int i = 0; i < ctx->win.ws_row; i++) {
    int y = i + ctx->offsetY;
    if (y >= ctx->len) {
      for (int j = 0; j < margin; j++) {
        frame[i][j] = (struct Cell){' ', RENDER_DEFAULT};
      }
      continue;
    }
    enum RenderMode mode = ctx->y == y ? RENDER_DEFAULT : RENDER_DIM;
    int len              = snprintf(buf, sizeof(buf), "%d", y + 1);
    for (int j = 0; j < len; j++) {
      frame[i][j] = (struct Cell){buf[j], mode};
    }
    for (int j = len; j < margin; j++) {
      frame[i][j] = (struct Cell){' ', mode};
    }
  }
}

void render_statusline(struct Context *ctx, struct Cell **frame) {
  char buf[MAX_BUFFER_SIZE];
  char *mode_label;
  int len;
  if (ctx->mode == MODE_COMMAND) {
    len = snprintf(buf, sizeof(buf), ":%s", ctx->cmd->buf);
  } else {
    if (ctx->mode == MODE_NORMAL) {
      mode_label = "NORMAL";
    } else if (ctx->mode == MODE_INSERT) {
      mode_label = "INSERT";
    }
    len = snprintf(buf, sizeof(buf), "-- %s -- %d/%d", mode_label, ctx->y + 1, ctx->x + 1);
  }
  int y = ctx->win.ws_row - 1;
  for (int i = 0; i < len; i++) {
    frame[y][i] = (struct Cell){buf[i], RENDER_DEFAULT};
  }
  for (int i = len; i < ctx->win.ws_col; i++) {
    frame[y][i] = (struct Cell){' ', RENDER_DEFAULT};
  }
}

void render_buf(struct Context *ctx, struct Cell **frame) {
  int margin = get_line_number_margin(ctx);
  for (int i = 0; i < ctx->win.ws_row; i++) {
    int y = ctx->offsetY + i;
    if (y >= ctx->len) {
      for (int j = margin; j < ctx->win.ws_col; j++) {
        frame[i][j] = (struct Cell){' ', RENDER_DEFAULT};
      }
      continue;
    }
    struct Line *line = ctx->buf[y];
    for (int j = margin; j < ctx->win.ws_col; j++) {
      int x       = ctx->offsetX + j - margin;
      frame[i][j] = (struct Cell){x < line->len ? line->buf[x] : ' ', RENDER_DEFAULT};
    }
  }
}

void render(struct Context *ctx) {
  struct Cell **frame = (struct Cell **)malloc(ctx->win.ws_row * sizeof(struct Cell *));
  for (int i = 0; i < ctx->win.ws_row; i++) {
    frame[i] = (struct Cell *)malloc(ctx->win.ws_col * sizeof(struct Cell));
  }
  render_line_number(ctx, frame);
  render_buf(ctx, frame);
  render_statusline(ctx, frame);
  for (int i = 0; i < ctx->win.ws_row; i++) {
    for (int j = 0; j < ctx->win.ws_col; j++) {
      if (ctx->prev_frame == NULL || frame[i][j].ch != ctx->prev_frame[i][j].ch || frame[i][j].mode != ctx->prev_frame[i][j].mode) {
        render_line(ctx, frame[i], ctx->win.ws_col, i);
        break;
      }
    }
  }
  if (ctx->prev_frame != NULL) {
    for (int i = 0; i < ctx->win.ws_row; i++) {
      free(ctx->prev_frame[i]);
    }
    free(ctx->prev_frame);
  }
  ctx->prev_frame = frame;
  if (ctx->mode == MODE_COMMAND) {
    move_cursor_yx(ctx->win.ws_row - 1, ctx->cmd->len + 1);
  } else {
    move_cursor_yx(ctx->y - ctx->offsetY, ctx->x - ctx->offsetX + get_line_number_margin(ctx));
  }
}
