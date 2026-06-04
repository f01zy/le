#ifndef RENDER_INCLUDED
#define RENDER_INCLUDED

#include "lexer.h"
#include "path.h"
#include "terminal.h"
#include "ui.h"

void render_line(struct Context *ctx, struct Cell *buf, size_t len, int y);
void render_tabmenu(struct Context *ctx, struct Cell **frame);
void render_line_numbers(struct Context *ctx, struct Document *doc, struct Cell **frame);
void render_statusline(struct Context *ctx, struct Document *doc, struct Cell **frame);
void render_buf(struct Context *ctx, struct Document *doc, struct Cell **frame);
void render_mappings_menu(struct Context *ctx, struct Document *doc, struct Cell **frame);
void render(struct Context *ctx);

#endif
