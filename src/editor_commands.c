#include <stdio.h>
#include <string.h>

#include "buffer.h"
#include "editor_commands.h"
#include "filesystem.h"
#include "service.h"

#define X(fullname, shortname) void cmd_##fullname(struct Context *ctx, char *token);
COMMANDS_LIST
#undef X

static struct Command commands_list[] = {
#define X(fullname, shortname) {#fullname, shortname, cmd_##fullname},
    COMMANDS_LIST
#undef X
};

void cmd_quit(struct Context *ctx, char *token) {
  for (int i = 0; i < ctx->len; i++) {
    if (ctx->docs[i]->is_changed) {
      unsaved_changes_dialog(ctx, set_flag_to_quit);
      return;
    }
  }
  set_flag_to_quit(ctx);
}

void cmd_quit_force(struct Context *ctx, char *token) { set_flag_to_quit(ctx); }

size_t _cmd_write(struct Document *doc, char *token) {
  token = strtok(NULL, " ");
  if (token) set_doc_path(doc, token);
  doc->is_changed = false;
  return save_doc(doc);
}

void cmd_write(struct Context *ctx, char *token) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  size_t size = _cmd_write(doc, token);
  if (size) {
    char buf[MAX_BUFFER_SIZE];
    snprintf(buf, sizeof(buf), "\"%s\", %zuB written", doc->path, size);
    set_statusline_message(ctx, buf, MESSAGE_INFO);
  } else {
    remove_doc_path(doc);
    set_statusline_message(ctx, "Failed to save file", MESSAGE_ERROR);
  }
}

void cmd_write_all(struct Context *ctx, char *token) {
  size_t size = 0;
  for (int i = 0; i < ctx->len; i++) {
    size += _cmd_write(ctx->docs[i], token);
  }
  char buf[MAX_BUFFER_SIZE];
  snprintf(buf, sizeof(buf), "%zuB written", size);
  set_statusline_message(ctx, buf, MESSAGE_INFO);
}

void cmd_edit(struct Context *ctx, char *token) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  token = strtok(NULL, " ");
  struct GetDocDataRes res = get_doc_data(token);
  if (!res.data) {
    set_statusline_message(ctx, "Failed to open file", MESSAGE_ERROR);
  } else {
    set_doc_path(doc, token);
    // TODO: Если до этого был открыт другой файл, надо очистить текущий буфер
    doc->buf = res.data;
    doc->len = res.len;
    doc->offset.x = doc->offset.y = 0;
    doc->pos.x = doc->pos.y = 0;
  }
}

void cmd_write_quit(struct Context *ctx, char *token) {
  cmd_write(ctx, token);
  cmd_quit(ctx, token);
}

void command_unknown(struct Context *ctx, char *token) {
  char buf[MAX_BUFFER_SIZE];
  size_t len = snprintf(buf, sizeof(buf), "Not an editor command: %s", token);
  set_statusline_message(ctx, buf, MESSAGE_ERROR);
}

void handle_command(struct Context *ctx) {
  size_t len = ctx->status.cmd.len;
  if (!len) return;
  char cmd[MAX_STRING_BUFFER_SIZE];
  memcpy(cmd, ctx->status.cmd.buf, sizeof(cmd));
  char *token = strtok(cmd, " ");

  bool is_found = false;
  for (int i = 0; i < sizeof(commands_list) / sizeof(commands_list[0]); i++) {
    if (!strcmp(commands_list[i].fullname, token) || !strcmp(commands_list[i].shortname, token)) {
      is_found = true;
      commands_list[i].act(ctx, token);
      break;
    }
  }

  if (!is_found) command_unknown(ctx, token);
}
