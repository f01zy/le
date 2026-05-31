#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editor_commands.h"

typedef void (*CommandAction)(struct Context *ctx, char *token);

struct {
  enum Command cmd;
  CommandAction act;
} commands_list[] = {
    {COMMAND_OPEN, command_open},
    {COMMAND_QUIT, command_quit},
    {COMMAND_SAVE, command_save},
    {COMMAND_UNKNOWN, command_unknown},
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

  enum Command cmd_type = COMMAND_UNKNOWN;
  if (!strcmp(token, "save")) cmd_type = COMMAND_SAVE;
  if (!strcmp(token, "open")) cmd_type = COMMAND_OPEN;
  if (!strcmp(token, "quit")) cmd_type = COMMAND_QUIT;

  for (int i = 0; i < sizeof(commands_list) / sizeof(commands_list[0]); i++) {
    if (commands_list[i].cmd == cmd_type) commands_list[i].act(ctx, token);
  }

  free(cmd);
}
