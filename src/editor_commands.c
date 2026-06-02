#include <stdio.h>
#include <string.h>

#include "editor_commands.h"
#include "service.h"

struct Command commands_list[] = {
    {"open", command_open},
    {"quit", command_quit},
    {"save", command_save},
};

void command_quit(struct Context *ctx, char *token) {
  for (int i = 0; i < ctx->len; i++) {
    if (ctx->docs[i]->is_changed) {
      unsaved_changes_dialog(ctx, set_flag_to_quit);
      return;
    }
  }
  set_flag_to_quit(ctx);
}

void command_save(struct Context *ctx, char *token) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  token = strtok(NULL, " ");
  if (!doc->path && token) set_doc_path(doc, token);
  int size = save_doc(doc);
  if (size == -1) {
    remove_doc_path(doc);
    set_statusline_message(ctx, "Failed to save file", MESSAGE_ERROR);
    return;
  }
  char buf[MAX_BUFFER_SIZE];
  snprintf(buf, sizeof(buf), "\"%s\", %dB written", doc->path, size);
  set_statusline_message(ctx, buf, MESSAGE_INFO);
}

void command_open(struct Context *ctx, char *token) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  token = strtok(NULL, " ");
  bool is_opened = load_doc_data(doc, token);
  if (!is_opened) {
    set_statusline_message(ctx, "Failed to open file", MESSAGE_ERROR);
  } else {
    doc->x = 0;
    doc->y = 0;
    doc->offsetX = 0;
    doc->offsetY = 0;
  }
}

void command_unknown(struct Context *ctx, char *token) {
  char buf[MAX_BUFFER_SIZE];
  int len = snprintf(buf, sizeof(buf), "Not an editor command: %s", token);
  set_statusline_message(ctx, buf, MESSAGE_ERROR);
}

void handle_command(struct Context *ctx) {
  int len = ctx->status.cmd.len;
  if (!len) return;
  char cmd[MAX_STRING_BUFFER_SIZE];
  memcpy(cmd, ctx->status.cmd.buf, sizeof(cmd));
  char *token = strtok(cmd, " ");

  bool is_found = false;
  for (int i = 0; i < sizeof(commands_list) / sizeof(commands_list[0]); i++) {
    if (!strcmp(commands_list[i].cmd, token)) {
      is_found = true;
      commands_list[i].act(ctx, token);
      break;
    }
  }

  if (!is_found) command_unknown(ctx, token);
}
