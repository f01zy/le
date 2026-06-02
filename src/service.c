#include <poll.h>
#include <stdio.h>
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
  set_editor_mode(ctx, EDITOR_MODE_NORMAL);
  set_statusline_mode(ctx, STATUS_MODE_NORMAL);
}

void quit_editor(struct Context *ctx) {
  free_resources(ctx);
  move_cursor_yx(0, 0);
  ANSI_RESET_SCREEN;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &ctx->backup);
}

void init_context(struct Context *ctx) {
  gettimeofday(&ctx->prev_frame_time, NULL);
  struct UI ui = {
      .is_line_numbers = true,
      .is_statusline = true,
      .is_tabmenu = true,
  };
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
  ctx->status.cmd.buf[0] = '\0';
  ctx->status.cmd.len = 0;
}

void free_mappings(struct Context *ctx, struct MappingNode *node) {
  for (int i = 0; i < node->len; i++) {
    free_mappings(ctx, node->nodes[i]);
  }
  free(node->nodes);
  free(node);
}

void free_resources(struct Context *ctx) {
  for (int i = 0; i < ctx->win.ws_row; i++) {
    free(ctx->prev_frame[i]);
    free(ctx->curr_frame[i]);
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
  free(ctx->curr_frame);
  free_mappings(ctx, ctx->head_mapping);
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

void set_editor_mode(struct Context *ctx, enum EditorMode mode) {
  enum CursorStyle style;
  if (mode == EDITOR_MODE_NORMAL) style = CURSOR_BLOCK_BLINKING;
  if (mode == EDITOR_MODE_INSERT) style = CURSOR_LINE_STATIC;
  if (mode == EDITOR_MODE_COMMAND) style = CURSOR_LINE_STATIC;
  ctx->mode = mode;
  set_cursor_style(style);
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

int getchar_nonblock(int ms) {
  struct pollfd pfd;
  pfd.fd = STDIN_FILENO;
  pfd.events = POLLIN;
  int ret = poll(&pfd, 1, ms);
  if (ret > 0 && (pfd.events & POLLIN)) return getchar();
  return -1;
}

void exec_curr_mapping(struct Context *ctx) {
  if (ctx->curr_mapping->act) ctx->curr_mapping->act(ctx);
  ctx->curr_mapping = ctx->head_mapping;
}

void set_statusline_message(struct Context *ctx, const char *msg, enum MessageLevel level) {
  set_statusline_mode(ctx, STATUS_MODE_MESSAGE);
  int len = MIN(strlen(msg), sizeof(ctx->status.msg.buf) - 1);
  memcpy(ctx->status.msg.buf, msg, len);
  ctx->status.msg.buf[len] = '\0';
  ctx->status.msg.level = level;
}

void set_statusline_dialog(struct Context *ctx, const char *question, void (*on_confirm)(struct Context *), void (*on_deny)(struct Context *)) {
  set_editor_mode(ctx, EDITOR_MODE_DIALOG);
  set_statusline_mode(ctx, STATUS_MODE_DIALOG);
  int len = MIN(strlen(question), sizeof(ctx->status.dialog.buf) - 1);
  memcpy(ctx->status.dialog.buf, question, len);
  ctx->status.dialog.buf[len] = '\0';
  ctx->status.dialog.on_confirm = on_confirm;
  ctx->status.dialog.on_deny = on_deny;
}

void unsaved_changes_dialog(struct Context *ctx, void (*on_confirm)(struct Context *ctx)) {
  set_statusline_dialog(ctx, "You have unsaved changes. Are you sure (Y/N): ", on_confirm, NULL);
}

void set_statusline_mode(struct Context *ctx, enum StatusMode mode) { ctx->status.mode = mode; }
void init_highlightings(struct Document *doc) { doc->tokens = scan_tokens(doc); }
void set_flag_to_quit(struct Context *ctx) { ctx->is_need_quit = true; }
