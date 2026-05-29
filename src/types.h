#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

#include <stddef.h>
#include <sys/ioctl.h>
#include <termios.h>

enum CursorStyle {
  CURSOR_BLOCK_BLINKING     = 1,
  CURSOR_BLOCK_STATIC       = 2,
  CURSOR_UNDERLINE_BLINKING = 3,
  CURSOR_UNDERLINE_STATIC   = 4,
  CURSOR_LINE_BLINKING      = 5,
  CURSOR_LINE_STATIC        = 6,
};

enum Mode {
  MODE_NORMAL,
  MODE_INSERT,
};

enum RemoveResult {
  REMOVE_NOTHING,
  REMOVE_CHAR,
  REMOVE_LINE,
};

struct Line {
  char *buf;
  size_t len;
  size_t size;
};

struct Context {
  int x, y;
  int offsetX, offsetY;
  struct Line **buf;
  size_t len;
  size_t size;
  struct termios conf;
  struct termios backup;
  struct winsize win;
  enum Mode mode;
};

#endif
