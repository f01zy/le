#ifndef EDITOR_COMMANDS_INCLUDED
#define EDITOR_COMMANDS_INCLUDED

#include "filesystem.h"
#include "service.h"

struct Command {
  const char *cmd;
  void (*act)(struct Context *ctx, char *token);
};

void handle_command(struct Context *ctx);

#endif
