#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "service.h"

void init_editor(struct Context *ctx) {
  ANSI_RESET_SCREEN;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ctx->win);
  tcgetattr(STDIN_FILENO, &ctx->backup);
  init_context(ctx);
  tcsetattr(STDIN_FILENO, TCSANOW, &ctx->conf);
  change_mode(ctx, MODE_NORMAL);
}

void quit_editor(struct Context *ctx) {
  free_resources(ctx);
  move_cursor_yx(0, 0);
  ANSI_RESET_SCREEN;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &ctx->backup);
}

void init_context(struct Context *ctx) {
  struct UI ui = {
      .is_line_numbers = true,
      .is_statusline = true,
      .is_tabmenu = true,
  };
  struct Line *cmd = (struct Line *)xmalloc(sizeof(struct Line));
  cmd->size = ctx->win.ws_col;
  cmd->buf = (char *)xmalloc(cmd->size);
  cmd->buf[0] = '\0';
  ctx->cmd = cmd;
  ctx->curr_frame = create_frame(ctx);
  ctx->prev_frame = create_frame(ctx);
  ctx->ui = ui;
  ctx->conf = ctx->backup;
  ctx->conf.c_iflag |= IXOFF;
  ctx->conf.c_iflag &= ~ICRNL;
  ctx->conf.c_lflag &= ~ECHO;
  ctx->conf.c_lflag &= ~ISIG;
  ctx->conf.c_lflag &= ~ICANON;
}

void clear_cmd(struct Context *ctx) {
  ctx->cmd->buf[0] = '\0';
  ctx->cmd->len = 0;
}

void free_resources(struct Context *ctx) {
  for (int i = 0; i < ctx->win.ws_row; i++) {
    free(ctx->prev_frame[i]);
  }
  for (int i = 0; i < ctx->len; i++) {
    struct Document *doc = ctx->docs[i];
    for (int j = 0; j < doc->len; j++) {
      struct Line *line = doc->buf[j];
      free(line->buf);
      free(line);
    }
    free(doc);
  }
  free(ctx->docs);
  free(ctx->prev_frame);
  free(ctx->cmd->buf);
  free(ctx->cmd);
}

void check_offset(struct Context *ctx, struct Document *doc) {
  int width = get_buffer_width(ctx);
  int height = get_buffer_height(ctx);

  if (doc->x < doc->offsetX) {
    doc->offsetX = doc->x;
  } else if (doc->x >= doc->offsetX + width) {
    doc->offsetX = doc->x - width + 1;
  }

  if (doc->y < doc->offsetY) {
    doc->offsetY = doc->y;
  } else if (doc->y >= doc->offsetY + height) {
    doc->offsetY = doc->y - height + 1;
  }
}

void change_mode(struct Context *ctx, enum Mode mode) {
  enum CursorStyle style;
  if (mode == MODE_NORMAL) style = CURSOR_BLOCK_BLINKING;
  if (mode == MODE_INSERT) style = CURSOR_LINE_STATIC;
  ctx->mode = mode;
  set_cursor_style(style);
}

void set_status(struct Context *ctx, char *msg, enum StatusType type) {
  if (!msg) return;
  if (ctx->status) {
    free(ctx->status->msg);
    free(ctx->status);
  }
  struct Status *status = (struct Status *)xmalloc(sizeof(struct Status));
  status->type = type;
  int len = strlen(msg);
  char *buf = (char *)xmalloc(len + 1);
  memcpy(buf, msg, len);
  buf[len] = '\0';
  status->msg = buf;
  ctx->status = status;
}

void clear_status(struct Context *ctx) {
  if (!ctx->status) return;
  free(ctx->status->msg);
  free(ctx->status);
  ctx->status = NULL;
}

struct Cell **create_frame(struct Context *ctx) {
  struct Cell **frame = (struct Cell **)xmalloc(ctx->win.ws_row * sizeof(struct Cell *));
  for (int i = 0; i < ctx->win.ws_row; i++) {
    frame[i] = (struct Cell *)xcalloc(ctx->win.ws_col, sizeof(struct Cell));
  }
  return frame;
}

const char *get_file_name(char *path) {
  if (!path || path[0] == '\0') return "New buffer";
  char *slash = strrchr(path, '/');
  if (!slash) return path;
  return slash + 1;
}
