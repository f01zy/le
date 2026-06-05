#ifndef UI_INCLUDED
#define UI_INCLUDED

#include "types.h"

enum CursorStyle {
  CURSOR_BLOCK_BLINKING = 1,
  CURSOR_BLOCK_STATIC = 2,
  CURSOR_UNDERLINE_BLINKING = 3,
  CURSOR_UNDERLINE_STATIC = 4,
  CURSOR_LINE_BLINKING = 5,
  CURSOR_LINE_STATIC = 6,
};

enum RenderMode {
  RENDER_DEFAULT,
  RENDER_BOLD,
  RENDER_DIM,
  RENDER_ITALIC,
  RENDER_UNDERLINE,
  RENDER_BLINKING,
  RENDER_INVERSE,
  RENDER_HIDDEN,
  RENDER_STRIKETHROUGH,
};

enum ForegroundColor {
  FOREGROUND_BLACK = 0,
  FOREGROUND_RED = 1,
  FOREGROUND_GREEN = 2,
  FOREGROUND_YELLOW = 3,
  FOREGROUND_BLUE = 4,
  FOREGROUND_MAGENTA = 5,
  FOREGROUND_CYAN = 6,
  FOREGROUND_WHITE = 7,
};

enum BackgroundColor {
  BACKGROUND_BLACK = 0,
  BACKGROUND_RED = 1,
  BACKGROUND_GREEN = 2,
  BACKGROUND_YELLOW = 3,
  BACKGROUND_BLUE = 4,
  BACKGROUND_MAGENTA = 5,
  BACKGROUND_CYAN = 6,
  BACKGROUND_WHITE = 7,
  BACKGROUND_GRAY = 8,
};

int get_statusline_margin(struct UI ui);
int get_tabmenu_margin(struct UI ui);
int get_line_number_margin(struct UI ui, size_t doc_len);

int get_buffer_width(struct UI ui, struct Vec2 size, size_t doc_len);
int get_buffer_height(struct UI ui, struct Vec2 size);

const char *get_editor_mode_label(enum EditorMode mode);
enum ForegroundColor get_token_foreground(enum TokenGroup group);

#endif
