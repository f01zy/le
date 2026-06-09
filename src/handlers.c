#include "handlers.h"
#include "buffer.h"
#include "editor_commands.h"
#include "mappings.h"
#include "service.h"

void handle_normal_mode(struct Context *ctx, int ch) {
  if (ch == KEY_ENTER && ctx->focus == EDITOR_FOCUS_TREE) {
    struct FileTreeEntity *ent = ctx->file_tree.labels[ctx->file_tree.pos].ent;
    if (ent->type == ENTITY_DIRECTORY) {
      ent->as.dir.is_open = !ent->as.dir.is_open;
    } else {
      cmd_edit(ctx, ent->path);
    }
    return;
  }
  if (ctx->mapping.len < sizeof(ctx->mapping.buf)) {
    ctx->mapping.buf[ctx->mapping.len++] = ch;
    ctx->mapping.buf[ctx->mapping.len] = '\0';
  }
  set_statusline_mode(ctx, STATUS_MODE_NORMAL);
  exec_mapping(ctx);
}

void handle_insert_mode(struct Context *ctx, int ch) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  doc->is_changed = true;

  switch (ch) {
  case KEY_ENTER:
    line_break(doc);
    doc->pos.y++;
    doc->pos.x = 0;
    break;

  case KEY_BACKSPACE:;
    int temp = doc->pos.y > 0 ? doc->buf[doc->pos.y - 1]->len : 0;
    enum RemoveResult res = remove_from_line(doc, doc->pos.y, doc->pos.x);
    if (res == REMOVE_CHAR) {
      doc->pos.x--;
    } else if (res == REMOVE_LINE) {
      doc->pos.x = temp;
      doc->pos.y--;
    }
    break;

  default:
    if (ch >= 32 && ch <= 126) {
      write_to_line(doc, doc->pos.y, doc->pos.x, ch);
      doc->pos.x++;
    }
    break;
  }
}

void handle_command_mode(struct Context *ctx, char ch) {
  switch (ch) {
  case KEY_ENTER:
    handle_command(ctx);
    if (ctx->mode != EDITOR_MODE_DIALOG) set_editor_mode(ctx, EDITOR_MODE_NORMAL);
    clear_cmd(ctx);
    break;

  case KEY_BACKSPACE:
    if (ctx->status.cmd.len > 0) ctx->status.cmd.buf[--ctx->status.cmd.len] = '\0';
    break;

  default:
    if (ch >= ALPHABET_MIN && ch <= ALPHABET_MAX && ctx->status.cmd.len < sizeof(ctx->status.cmd.buf) - 1) {
      ctx->status.cmd.buf[ctx->status.cmd.len++] = ch;
      ctx->status.cmd.buf[ctx->status.cmd.len] = '\0';
    }
    break;
  }
}

void handle_dialog_mode(struct Context *ctx, char ch) {
  if (tolower(ch) == 'y') {
    if (ctx->status.dialog.on_confirm) ctx->status.dialog.on_confirm(ctx);
  } else {
    if (ctx->status.dialog.on_deny) ctx->status.dialog.on_deny(ctx);
  }
  set_editor_mode(ctx, EDITOR_MODE_NORMAL);
  set_statusline_mode(ctx, STATUS_MODE_NORMAL);
}
