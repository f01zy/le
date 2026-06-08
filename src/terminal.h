#ifndef TERMINAL_INCLUDED
#define TERMINAL_INCLUDED

#include "types.h"
#include "ui.h"

struct Vec2 get_terminal_size();
void set_cursor_style(enum CursorStyle type);
void set_render_mode(enum RenderMode render_mode, enum ForegroundColor fg, enum BackgroundColor bg);
void move_cursor_yx(int y, int x);

#endif
