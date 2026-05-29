#ifndef SERVICE_INCLUDED
#define SERVICE_INCLUDED

#include "types.h"

void configure_context(struct Context *ctx);
void add_line(struct Context *ctx, int y);
void remove_line(struct Context *ctx, int y);
void write_to_line(struct Context *ctx, int y, int x, char ch);
void remove_curr(struct Context *ctx);
void line_break(struct Context *ctx, int y);
void move_cursor_yx(int y, int x);

#endif
