#include "handlers.h"
#include <stdio.h>

struct Context ctx;

int main(int argc, char **argv) {
  if (argc > 2) {
    printf("Invalid input data\n");
    return 1;
  }
  init_editor(&ctx);
  if (argc == 2) {
    struct Document *doc = create_doc(&ctx);
    load_doc_data(doc, argv[1]);
  } else {
    struct Document *doc = create_doc(&ctx);
    add_line(doc, NULL, 0);
  }
  render(&ctx);

  int ch;
  while (!ctx.is_exit) {
    ch = getchar();
    if (ctx.mode == MODE_INSERT) {
      handle_insert_mode(&ctx, ch);
    } else if (ctx.mode == MODE_COMMAND) {
      handle_command_mode(&ctx, ch);
    } else if (ctx.mode == MODE_NORMAL) {
      handle_normal_mode(&ctx, ch);
    }

    struct Document *doc = ctx.docs[ctx.curr_doc];
    check_offset(&ctx, doc);
    render(&ctx);
    clear_status(&ctx);
  }

  quit_editor(&ctx);
  return 0;
}
