#ifndef RENDER_INCLUDED
#define RENDER_INCLUDED

#include "path.h"
#include "selected.h"
#include "terminal.h"

void render_line(struct Cell *buf, size_t len, int y);
void render_tabmenu(struct Context *ctx, struct Cell **frame);
void render_line_numbers(struct Context *ctx, struct Cell **frame);
void render_statusline(struct Context *ctx, struct Cell **frame);
void render_buf(struct Context *ctx, struct Cell **frame);
void render(struct Context *ctx);

#endif
