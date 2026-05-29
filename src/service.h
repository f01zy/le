#ifndef SERVICE_INCLUDED
#define SERVICE_INCLUDED

#include "types.h"

enum RemoveResult remove_from_line(struct Context *ctx, int y, int x);
void change_mode(struct Context *ctx, enum Mode mode);
void set_cursor_type(enum CursorStyle type);
void configure_context(struct Context *ctx);
void add_line(struct Context *ctx, int y);
void remove_line(struct Context *ctx, int y);
void write_to_line(struct Context *ctx, int y, int x, char ch);
void line_break(struct Context *ctx);
void move_cursor_yx(int y, int x);

#endif
