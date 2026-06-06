#include "mappings.h"
#include <string.h>

#define X(name, desc, mapping) void cmd_##name(struct Context *ctx);
COMMANDS_LIST
#undef X

static struct Mapping mappings_list[] = {
#define X(name, desc, mapping) {mapping, desc, cmd_##name},
    COMMANDS_LIST
#undef X
};

// Movement
void cmd_up(struct Context *ctx, int count) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  if (doc->pos.y < count) return;
  doc->pos.x = MIN(doc->pos.x, MAX_LINE_X(doc->buf[doc->pos.y - count]->len));
  doc->pos.y -= count;
}

void cmd_down(struct Context *ctx, int count) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  if (doc->pos.y + count >= doc->len) return;
  doc->pos.x = MIN(doc->pos.x, MAX_LINE_X(doc->buf[doc->pos.y + count]->len));
  doc->pos.y += count;
}

void cmd_left(struct Context *ctx, int count) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  struct Vec2 pos = doc->pos;
  while (pos.y >= 0) {
    struct Line *line = doc->buf[pos.y];
    if (count > pos.x) {
      if (!pos.y) {
        pos.x = 0;
        break;
      }
      count -= pos.x + 1;
      pos.x = MAX_LINE_X(doc->buf[pos.y - 1]->len);
      pos.y--;
    } else {
      pos.x -= count;
      break;
    }
  }
  doc->pos = pos;
}

void cmd_right(struct Context *ctx, int count) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  struct Vec2 pos = doc->pos;
  while (pos.y < doc->len) {
    struct Line *line = doc->buf[pos.y];
    size_t len = MAX_LINE_X(line->len) - pos.x;
    if (count > len) {
      if (pos.y == doc->len - 1) {
        pos.x = MAX_LINE_X(line->len);
        break;
      }
      count -= len + 1;
      pos.x = 0;
      pos.y++;
    } else {
      pos.x += count;
      break;
    }
  }
  doc->pos = pos;
}

// Lines
void cmd_line_start(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  doc->pos.x = 0;
}

void cmd_line_end(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  doc->pos.x = MAX_LINE_X(doc->buf[doc->pos.y]->len);
}

// Documents
void cmd_doc_start(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  doc->pos.x = MIN(doc->pos.x, MAX_LINE_X(doc->buf[0]->len));
  doc->pos.y = 0;
}

void cmd_doc_end(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  doc->pos.x = MIN(doc->pos.x, MAX_LINE_X(doc->buf[doc->len - 1]->len));
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
void cmd_toggle_code_highlighting(struct Context *ctx) { ctx->ui.is_code_highlighting = !ctx->ui.is_code_highlighting; }

// Other
void exec_mapping(struct Context *ctx) {
  struct Document *doc = ctx->docs[ctx->curr_doc];
  struct MappingNode static_mapping;
  enum ParsingStatus static_status = parse_static_mapping(ctx, &static_mapping);
  if (static_status == PARSING_STATUS_WAITING) return;
  if (static_status == PARSING_STATUS_SUCCESS) {
    if (static_mapping.act) static_mapping.act(ctx);
    reset_curr_mapping(ctx);
    return;
  }

  struct DinamicMapping dinamic_mapping;
  if (parse_dinamic_mapping(ctx, &dinamic_mapping) == PARSING_STATUS_ERROR) return;
  struct Vec4 c = get_motion_object_bounds(doc, ctx->mode, dinamic_mapping);
  size_t count = dinamic_mapping.global_count;

  switch (dinamic_mapping.op) {
  case 'd':
  case 'y':
  case 'c': {
    char buf[MAX_BUFFER_SIZE];
    get_selected_buffer(doc, buf, sizeof(buf));
    enum EditorMode mode = EDITOR_MODE_NORMAL;
    if (dinamic_mapping.op == 'd') {
      remove_range(doc, c);
      doc->pos.x = MIN(doc->pos.x, MAX_LINE_X(doc->buf[doc->pos.y]->len));
    }
    if (dinamic_mapping.op == 'c') {
      mode = EDITOR_MODE_INSERT;
      remove_range(doc, c);
      doc->pos.x = MIN(doc->pos.x, MAX_LINE_X(doc->buf[doc->pos.y]->len));
    }
    if (ctx->mode == EDITOR_MODE_VISUAL) {
      doc->pos.x = MIN(MAX_LINE_X(doc->buf[doc->selected.y]->len), doc->selected.x);
      doc->pos.y = doc->selected.y;
    }
    copy_to_clipboard(buf);
    set_editor_mode(ctx, mode);
    break;
  }

  case 'l':
    cmd_right(ctx, count);
    break;
  case 'h':
    cmd_left(ctx, count);
    break;
  case 'j':
    cmd_down(ctx, count);
    break;
  case 'k':
    cmd_up(ctx, count);
    break;
  }

  init_tokens(doc);
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
  ctx->mapping.head = head;
}
