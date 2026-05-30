#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <termios.h>

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

enum CursorStyle {
  CURSOR_BLOCK_BLINKING = 1,
  CURSOR_BLOCK_STATIC,
  CURSOR_UNDERLINE_BLINKING,
  CURSOR_UNDERLINE_STATIC,
  CURSOR_LINE_BLINKING,
  CURSOR_LINE_STATIC,
};

enum Mode { MODE_NORMAL, MODE_INSERT, MODE_COMMAND };
enum RemoveResult { REMOVE_NOTHING, REMOVE_CHAR, REMOVE_LINE };

struct UI {
  bool is_line_number;
};

struct Line {
  char *buf;
  size_t len;
  size_t size;
};

struct Cell {
  char ch;
  enum RenderMode mode;
};

struct Context {
  int x, y;
  int offsetX, offsetY;
  struct Cell **prev_frame;
  struct Line **buf;
  struct Line *cmd;
  size_t len;
  size_t size;
  struct termios conf;
  struct termios backup;
  struct winsize win;
  struct UI ui;
  enum Mode mode;
  bool is_exit;
};

#endif
