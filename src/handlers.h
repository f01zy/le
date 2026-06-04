#ifndef HANDLERS_INCLUDED
#define HANDLERS_INCLUDED

#include "editor_commands.h"
#include "mappings.h"

void handle_dialog_mode(struct Context *ctx, char ch);
void handle_command_mode(struct Context *ctx, char ch);
void handle_insert_mode(struct Context *ctx, int ch);
void handle_normal_mode(struct Context *ctx, int ch);

#endif
