#include "buffer.h"
#include "file_tree.h"
#include "handlers.h"
#include "mappings.h"
#include "render.h"
#include "service.h"

struct Context ctx;

int main(int argc, char **argv) {
  init_editor(&ctx);
  init_mappings(&ctx);
  init_file_tree(&ctx);
  init_file_tree_labels(&ctx);

  struct Document *doc = create_doc(&ctx);
  if (argc == 2) {
    init_doc(doc, argv[1]);
    init_tokens(doc);
  }
  render(&ctx);

  int ch;
  while (!ctx.is_need_quit) {
    struct Document *doc = ctx.docs[ctx.curr_doc];
    struct timeval now;
    gettimeofday(&now, NULL);
    __suseconds_t delta = (now.tv_sec - ctx.frame.prev_time.tv_sec) * 1000000LL + (now.tv_usec - ctx.frame.prev_time.tv_usec);
    ch = getchar_nonblock(RENDER_DELAY);

    if (ch == KEY_ESCAPE) {
      if (doc->pos.x) doc->pos.x--;
      reset_curr_mapping(&ctx);
      set_editor_mode(&ctx, EDITOR_MODE_NORMAL);
      set_statusline_mode(&ctx, STATUS_MODE_NORMAL);
      clear_cmd(&ctx);
    }

    if (ctx.mapping.len && delta > MAPPING_DELAY) {
      struct MappingNode node;
      if (parse_static_mapping(&ctx, &node) != PARSING_STATUS_ERROR) {
        if (node.act) node.act(&ctx);
        reset_curr_mapping(&ctx);
      }
      render(&ctx);
    }

    if (ch != -1) {
      ctx.frame.prev_time = now;
      switch (ctx.mode) {
      case EDITOR_MODE_INSERT:
        handle_insert_mode(&ctx, ch);
        break;
      case EDITOR_MODE_VISUAL:
      case EDITOR_MODE_NORMAL:
        handle_normal_mode(&ctx, ch);
        break;
      case EDITOR_MODE_COMMAND:
        handle_command_mode(&ctx, ch);
        break;
      case EDITOR_MODE_DIALOG:
        handle_dialog_mode(&ctx, ch);
        break;
      }
      if (ctx.focus == EDITOR_FOCUS_TREE) {
        init_file_tree_labels(&ctx);
        update_tree_offset(&ctx);
      }
      if (ctx.focus == EDITOR_FOCUS_BUFFER) {
        if (!is_sticky_motion(ch)) update_doc_max_x(doc);
        update_doc_offset(doc, ctx.ui, ctx.terminal.size);
      }
      init_tokens(doc);
      render(&ctx);
    }
  }

  quit_editor(&ctx);
  return 0;
}
