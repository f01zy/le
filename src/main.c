#include <stdio.h>

#include "handlers.h"
#include "mappings.h"

struct Context ctx;

int main(int argc, char **argv) {
  if (argc > 2) {
    printf("Invalid input data\n");
    return 1;
  }
  init_editor(&ctx);
  init_mappings(&ctx);
  struct Document *doc = create_doc(&ctx);
  if (argc == 2) {
    load_doc_data(doc, argv[1]);
    init_highlightings(doc);
  }
  render(&ctx);

  int ch;
  while (!ctx.is_need_quit) {
    struct Document *doc = ctx.docs[ctx.curr_doc];
    struct timeval now;
    gettimeofday(&now, NULL);
    __suseconds_t delta = (now.tv_sec - ctx.prev_frame_time.tv_sec) * 1000000LL + (now.tv_usec - ctx.prev_frame_time.tv_usec);
    ch = getchar_nonblock(20);

    if (ch == KEY_ESCAPE) {
      reset_curr_mapping(&ctx);
      if (doc->x) doc->x--;
      set_editor_mode(&ctx, EDITOR_MODE_NORMAL);
      set_statusline_mode(&ctx, STATUS_MODE_NORMAL);
      clear_cmd(&ctx);
    }
    if (!ctx.ui.is_mappings_menu && delta > 200000) {
      exec_curr_mapping(&ctx);
      render(&ctx);
    }
    if (ch != -1) {
      ctx.prev_frame_time = now;
      switch (ctx.mode) {
      case EDITOR_MODE_INSERT:
        init_highlightings(doc);
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
      check_offset(&ctx, doc);
      render(&ctx);
    }
  }

  quit_editor(&ctx);
  return 0;
}
