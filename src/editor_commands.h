#ifndef EDITOR_COMMANDS_INCLUDED
#define EDITOR_COMMANDS_INCLUDED

#include "service.h"

void command_unknown(struct Context *ctx, char *token);
void command_quit(struct Context *ctx, char *token);
void command_save(struct Context *ctx, char *token);
void command_open(struct Context *ctx, char *token);

void handle_command(struct Context *ctx);

#endif
