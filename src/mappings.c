#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "mappings.h"

static struct Mapping mappings_list[] = {
#define X(name, desc, mapping) {mapping, desc, cmd_##name},
    COMMANDS_LIST
#undef X
};

// Movement
void cmd_up(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  if (!doc->pos.y) return;
  doc->pos.x = MIN(doc->pos.x, get_max_x(doc->buf[doc->pos.y - 1]));
  doc->pos.y--;
}

void cmd_down(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  if (doc->pos.y == doc->len - 1) return;
  doc->pos.x = MIN(doc->pos.x, get_max_x(doc->buf[doc->pos.y + 1]));
  doc->pos.y++;
}

void cmd_left(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  if (!doc->pos.x && !doc->pos.y) return;
  if (!doc->pos.x) {
    doc->pos.x = get_max_x(doc->buf[doc->pos.y - 1]);
    doc->pos.y--;
  } else {
    doc->pos.x--;
  }
}

void cmd_right(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  size_t len = get_max_x(doc->buf[doc->pos.y]);
  if (doc->pos.x == len && doc->pos.y == doc->len - 1) return;
  if (doc->pos.x == len) {
    doc->pos.x = 0;
    doc->pos.y++;
  } else {
    doc->pos.x++;
  }
}

// Lines
void cmd_line_start(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  doc->pos.x = 0;
}

void cmd_line_end(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  doc->pos.x = get_max_x(doc->buf[doc->pos.y]);
}

// Documents
void cmd_doc_start(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  doc->pos.x = MIN(doc->pos.x, get_max_x(doc->buf[0]));
  doc->pos.y = 0;
}

void cmd_doc_end(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  doc->pos.x = MIN(doc->pos.x, get_max_x(doc->buf[doc->len - 1]));
  doc->pos.y = doc->len - 1;
}

void cmd_doc_prev(struct Context *ctx) {
  if (!ctx->curr_doc) return;
  ctx->curr_doc--;
}

void cmd_doc_next(struct Context *ctx) {
  if (ctx->curr_doc == ctx->len - 1) return;
  ctx->curr_doc++;
}

void cmd_doc_new(struct Context *ctx) {
  if (!ctx->docs[ctx->len - 1]->path) return;
  create_doc(ctx);
  ctx->curr_doc = ctx->len - 1;
}

void _cmd_doc_close(struct Context *ctx) {
  if (ctx->len == 1) return;
  ctx->len--;
  for (int i = ctx->curr_doc; i < ctx->len; i++) {
    ctx->docs[i] = ctx->docs[i + 1];
  }
  ctx->curr_doc = MIN(ctx->curr_doc, ctx->len - 1);
}

void cmd_doc_close(struct Context *ctx) {
  if (ctx->docs[ctx->curr_doc]->is_changed) {
    unsaved_changes_dialog(ctx, _cmd_doc_close);
    return;
  }
  _cmd_doc_close(ctx);
}

// Editor modes
void cmd_command_mode(struct Context *ctx) {
  set_editor_mode(ctx, EDITOR_MODE_COMMAND);
  set_statusline_mode(ctx, STATUS_MODE_COMMAND);
}

void cmd_insert_mode_prev(struct Context *ctx) { set_editor_mode(ctx, EDITOR_MODE_INSERT); }

void cmd_insert_mode_next(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  if (doc->buf[doc->pos.y]->len > 0) doc->pos.x++;
  set_editor_mode(ctx, EDITOR_MODE_INSERT);
}

void cmd_visual_mode(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  doc->selected.x = doc->pos.x;
  doc->selected.y = doc->pos.y;
  set_editor_mode(ctx, EDITOR_MODE_VISUAL);
}

// UI
void cmd_toggle_line_numbers(struct Context *ctx) { ctx->ui.is_line_numbers = !ctx->ui.is_line_numbers; }
void cmd_toggle_relative_line_numbers(struct Context *ctx) { ctx->ui.is_relative_line_numbers = !ctx->ui.is_relative_line_numbers; }
void cmd_toggle_mappings_menu(struct Context *ctx) { ctx->ui.is_mappings_menu = true; }
void cmd_toggle_code_highlighting(struct Context *ctx) { ctx->ui.is_code_highlighting = !ctx->ui.is_code_highlighting; }

// Visual mode
void cmd_yank(struct Context *ctx) {
  if (ctx->mode != EDITOR_MODE_VISUAL) return;
  struct Document *doc = ctx->docs[ctx->curr_doc];
  char buf[MAX_BUFFER_SIZE];
  get_selected_buffer(doc, buf, sizeof(buf));
  doc->pos.x = MIN(doc->buf[doc->selected.y]->len - 1, doc->selected.x);
  doc->pos.y = doc->selected.y;
  copy_to_clipboard(buf);
  set_editor_mode(ctx, EDITOR_MODE_NORMAL);
}

void cmd_delete(struct Context *ctx) {
  if (ctx->mode != EDITOR_MODE_VISUAL) return;
  struct Document *doc = ctx->docs[ctx->curr_doc];
  remove_range(doc, (struct Vec4){doc->selected.x, doc->selected.y, doc->pos.x, doc->pos.y});
  doc->pos.x = MIN(doc->buf[doc->selected.y]->len - 1, doc->selected.x);
  doc->pos.y = doc->selected.y;
  set_editor_mode(ctx, EDITOR_MODE_NORMAL);
  init_tokens(doc);
}

// TODO: разделить построчное удаление для операторов движения и повторения от посимвольного удаления для прочих объектов
void exec_dinamic_mapping(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  struct ParsedMapping mapping = parse_dinamic_mapping(ctx->mapping.buf, ctx->mapping.len);
  struct Vec4 c = get_motion_object_bounds(doc, mapping);
  if (c.ax == -1) {
    reset_curr_mapping(ctx);
    return;
  }
  switch (mapping.op) {
  case 'd':
    remove_range(doc, c);
    break;
  }
  reset_curr_mapping(ctx);
}

void add_mapping_node(struct Context *ctx, struct MappingNode *head, struct Mapping map) {
  struct MappingNode *curr = head;
  size_t len = strlen(map.cmd);
  for (int i = 0; i < len; i++) {
    bool is_found = false;
    for (int j = 0; j < curr->len; j++) {
      if (curr->nodes[j]->ch == map.cmd[i]) {
        is_found = true;
        curr = curr->nodes[j];
        break;
      }
    }
    if (!is_found) {
      struct MappingNode *node = (struct MappingNode *)xcalloc(1, sizeof(struct MappingNode));
      node->ch = map.cmd[i];
      curr->len++;
      curr->nodes = (struct MappingNode **)xrealloc(curr->nodes, curr->len * sizeof(struct MappingNode *));
      curr->nodes[curr->len - 1] = node;
      curr = node;
    }
    if (i == len - 1) {
      curr->act = map.act;
      curr->desc = map.desc;
    }
  }
}

void init_mappings(struct Context *ctx) {
  struct MappingNode *head = (struct MappingNode *)xcalloc(1, sizeof(struct MappingNode));
  for (int i = 0; i < sizeof(mappings_list) / sizeof(mappings_list[0]); i++) {
    struct Mapping map = mappings_list[i];
    add_mapping_node(ctx, head, map);
  }
  ctx->mapping.head_mapping = ctx->mapping.curr_mapping = head;
}
