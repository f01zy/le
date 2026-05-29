#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

#include <stddef.h>
#include <sys/ioctl.h>
#include <termios.h>

enum Mode { MODE_NORMAL, MODE_INSERT };
enum RemoveResult { REMOVE_NOTHING, REMOVE_CHAR, REMOVE_LINE };

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
