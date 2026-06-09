#ifndef EDITOR_COMMANDS_INCLUDED
#define EDITOR_COMMANDS_INCLUDED

#include "types.h"

struct Command {
  const char *fullname;
  const char *shortname;
  void (*act)(struct Context *ctx, char *token);
};

#define COMMANDS_LIST                                                                                                                                          \
  X(edit, "e")                                                                                                                                                 \
  X(quit, "q")                                                                                                                                                 \
  X(quit_force, "q!")                                                                                                                                          \
  X(write, "w")                                                                                                                                                \
  X(write_quit, "wq")                                                                                                                                          \
  X(write_all, "wa")

void cmd_edit(struct Context *ctx, char *path);
void handle_command(struct Context *ctx);

#endif
