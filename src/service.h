#ifndef SERVICE_INCLUDED
#define SERVICE_INCLUDED

#include "types.h"

void configure_context(struct Context *ctx);

void free_resources(struct Context *ctx);
void clear_cmd(struct Context *ctx);
void move_cursor_yx(int y, int x);
void check_offset(struct Context *ctx);
void change_mode(struct Context *ctx, enum Mode mode);
size_t get_max_x(struct Line *line);
int get_line_number_margin(struct Context *ctx);
void set_cursor_style(enum CursorStyle type);
void set_render_mode(enum RenderMode mode);

enum RemoveResult remove_from_line(struct Context *ctx, int y, int x);
void add_line(struct Context *ctx, int y);
void remove_line(struct Context *ctx, int y);
void write_to_line(struct Context *ctx, int y, int x, char ch);
void line_break(struct Context *ctx);

#endif
