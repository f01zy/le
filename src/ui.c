#include "ui.h"

int get_line_number_margin(struct UI ui, size_t doc_len) {
  if (!ui.is_line_numbers) return 0;
  int margin = 0;
  while (doc_len) {
    margin++;
    doc_len /= 10;
  }
  return margin + 1;
}

int get_statusline_margin(struct UI ui) {
  if (!ui.is_statusline) return 0;
  return 1;
}

int get_tabmenu_margin(struct UI ui) {
  if (!ui.is_tabmenu) return 0;
  return 1;
}

int get_buffer_width(struct UI ui, struct Vec2 size, size_t doc_len) { return size.x - get_line_number_margin(ui, doc_len); }
int get_buffer_height(struct UI ui, struct Vec2 size) { return size.y - get_tabmenu_margin(ui) - get_statusline_margin(ui); }

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

const char *get_editor_mode_label(enum EditorMode mode) {
  switch (mode) {
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
