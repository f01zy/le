#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editor_commands.h"

struct Command commands_list[] = {
    {"open", command_open},
    {"quit", command_quit},
    {"save", command_save},
};

void command_quit(struct Context *ctx, char *token) { ctx->is_exit = true; }

void command_save(struct Context *ctx, char *token) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  int size = save_doc(doc);
  if (size == -1) {
    set_status(ctx, "Failed to save file", STATUS_ERROR);
    return;
  }
  char buf[MAX_BUFFER_SIZE];
  snprintf(buf, sizeof(buf), "\"%s\", %dB written", doc->path, size);
  set_status(ctx, buf, STATUS_INFO);
}

void command_open(struct Context *ctx, char *token) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  token = strtok(NULL, " ");
  bool is_opened = load_doc_data(doc, token);
  if (!is_opened) {
    set_status(ctx, "Failed to open file", STATUS_ERROR);
    return;
  }
}

void command_unknown(struct Context *ctx, char *token) {
  char buf[MAX_BUFFER_SIZE];
  int len = snprintf(buf, sizeof(buf), "Not an editor command: %s", token);
  set_status(ctx, buf, STATUS_ERROR);
}

void handle_command(struct Context *ctx) {
  if (!ctx->cmd && !ctx->cmd->len) return;
  char *cmd = (char *)xmalloc(ctx->cmd->len + 1);
  memcpy(cmd, ctx->cmd->buf, ctx->cmd->len);
  cmd[ctx->cmd->len] = '\0';
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
  free(cmd);
}
