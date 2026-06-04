#include "ui.h"

int get_line_number_margin(struct Context *ctx) {
  if (!ctx->ui.is_line_numbers) return 0;
  struct Document *doc = ctx->docs[ctx->curr_doc];
  int len = doc->len, margin = 0;
  while (len) {
    margin++;
    len /= 10;
  }
  return margin + 1;
}

int get_statusline_margin(struct Context *ctx) {
  if (!ctx->ui.is_statusline) return 0;
  return 1;
}

int get_tabmenu_margin(struct Context *ctx) {
  if (!ctx->ui.is_tabmenu) return 0;
  return 1;
}

int get_buffer_width(struct Context *ctx) { return ctx->win.ws_col - get_line_number_margin(ctx); }
int get_buffer_height(struct Context *ctx) { return ctx->win.ws_row - get_tabmenu_margin(ctx) - get_statusline_margin(ctx); }

enum ForegroundColor get_token_foreground(enum TokenGroup group) {
  switch (group) {
  case TOKEN_LITERAL_STRING:
    return FOREGROUND_YELLOW;

  case TOKEN_LITERAL_NUMBER:
  case TOKEN_DATA_TYPE:
    return FOREGROUND_BLUE;

  case TOKEN_LITERAL_SYMBOL:
  case TOKEN_KEYWORD:
    return FOREGROUND_MAGENTA;

  case TOKEN_FUNCTION:
  case TOKEN_DIRECTIVE:
    return FOREGROUND_GREEN;

  case TOKEN_DELIMITER:
    return FOREGROUND_RED;

  default:
    return FOREGROUND_WHITE;
  }
}

const char *get_editor_mode_label(struct Context *ctx) {
  switch (ctx->mode) {
  case EDITOR_MODE_INSERT:
    return "INSERT";
  case EDITOR_MODE_VISUAL:
    return "VISUAL";
    break;
  default:
    return "NORMAL";
    break;
  }
}

void get_selected_coordinates(int *ay, int *ax, int *by, int *bx) {
  int is_single_line = (*ay == *by);
  int minY = (*ay > *by) ? *by : *ay;
  int maxY = (*ay > *by) ? *ay : *by;
  int minX = is_single_line ? MIN(*ax, *bx) : ((*ay > *by) ? *bx : *ax);
  int maxX = is_single_line ? MAX(*ax, *bx) : ((*ay > *by) ? *ax : *bx);
  *ay = minY, *ax = minX;
  *by = maxY, *bx = maxX;
}
