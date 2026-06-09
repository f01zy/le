#include "ui.h"

struct Vec2 get_file_tree_size(struct UI ui, struct Vec2 term_size) {
  int width = ui.is_file_tree ? FILE_TREE_WIDTH : 0;
  int height = term_size.y - get_statusline_height(ui);
  return (struct Vec2){width, height};
}

struct Vec2 get_file_tree_offset(struct UI ui, struct Vec2 term_size) {
  int offsetX = 0;
  int offsetY = 0;
  return (struct Vec2){offsetX, offsetY};
}

struct Vec2 get_line_numbers_size(struct UI ui, struct Vec2 term_size, size_t doc_len) {
  int height = term_size.y - get_tabmenu_size(ui, term_size).y - get_statusline_height(ui);
  int width = 0;
  if (ui.is_line_numbers) {
    while (doc_len) {
      width++;
      doc_len /= 10;
    }
    width++;
  }
  return (struct Vec2){width, height};
}

struct Vec2 get_line_numbers_offset(struct UI ui, struct Vec2 term_size, size_t doc_len) {
  int offsetX = get_file_tree_size(ui, term_size).x;
  int offsetY = get_tabmenu_size(ui, term_size).y;
  return (struct Vec2){offsetX, offsetY};
}

struct Vec2 get_buf_size(struct UI ui, struct Vec2 term_size, size_t doc_len) {
  int width = term_size.x - get_file_tree_size(ui, term_size).x - get_line_numbers_size(ui, term_size, doc_len).x;
  int height = term_size.y - get_tabmenu_size(ui, term_size).y - get_statusline_height(ui);
  return (struct Vec2){width, height};
}

struct Vec2 get_buf_offset(struct UI ui, struct Vec2 term_size, size_t doc_len) {
  int offsetX = get_file_tree_size(ui, term_size).x + get_line_numbers_size(ui, term_size, doc_len).x;
  int offsetY = get_tabmenu_size(ui, term_size).y;
  return (struct Vec2){offsetX, offsetY};
}

struct Vec2 get_tabmenu_size(struct UI ui, struct Vec2 term_size) {
  int width = term_size.x - get_file_tree_size(ui, term_size).x;
  int height = ui.is_tabmenu;
  return (struct Vec2){width, height};
}

struct Vec2 get_tabmenu_offset(struct UI ui, struct Vec2 term_size) {
  int offsetX = get_file_tree_size(ui, term_size).x;
  int offsetY = 0;
  return (struct Vec2){offsetX, offsetY};
}

int get_statusline_height(struct UI ui) {
  if (!ui.is_statusline) return 0;
  return 1;
}

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
  default:
    return "NORMAL";
  }
}
