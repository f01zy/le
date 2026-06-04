#ifndef UI_INCLUDED
#define UI_INCLUDED

#include "types.h"

int get_line_number_margin(struct Context *ctx);
int get_statusline_margin(struct Context *ctx);
int get_tabmenu_margin(struct Context *ctx);

int get_buffer_width(struct Context *ctx);
int get_buffer_height(struct Context *ctx);

void get_selected_coordinates(int *ay, int *ax, int *by, int *bx);
const char *get_editor_mode_label(struct Context *ctx);
enum ForegroundColor get_token_foreground(enum TokenGroup group);

#endif
