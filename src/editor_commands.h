#ifndef EDITOR_COMMANDS_INCLUDED
#define EDITOR_COMMANDS_INCLUDED

#include "filesystem.h"
#include "service.h"

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

void handle_command(struct Context *ctx);

#endif
