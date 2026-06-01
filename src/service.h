#ifndef SERVICE_INCLUDED
#define SERVICE_INCLUDED

#include "buffer.h"
#include "terminal.h"
#include "ui.h"

void set_status(struct Context *ctx, char *msg, enum StatusType type);
void clear_status(struct Context *ctx);

void init_context(struct Context *ctx);
void init_editor(struct Context *ctx);
void quit_editor(struct Context *ctx);

struct Cell **create_frame(struct Context *ctx);
void free_resources(struct Context *ctx);
void clear_cmd(struct Context *ctx);
void check_offset(struct Context *ctx, struct Document *doc);
void change_mode(struct Context *ctx, enum Mode mode);
const char *get_file_name(char *path);

#endif
