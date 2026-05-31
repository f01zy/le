#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <termios.h>

enum CursorStyle {
  CURSOR_BLOCK_BLINKING = 1,
  CURSOR_BLOCK_STATIC,
  CURSOR_UNDERLINE_BLINKING,
  CURSOR_UNDERLINE_STATIC,
  CURSOR_LINE_BLINKING,
  CURSOR_LINE_STATIC,
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
  FOREGROUND_BLACK = 30,
  FOREGROUND_RED,
  FOREGROUND_GREEN,
  FOREGROUND_YELLOW,
  FOREGROUND_BLUE,
  FOREGROUND_MAGENTA,
  FOREGROUND_CYAN,
  FOREGROUND_WHITE,
  FOREGROUND_DEFAULT = 39,
};

enum BackgroundColor {
  BACKGROUND_BLACK = 40,
  BACKGROUND_RED,
  BACKGROUND_GREEN,
  BACKGROUND_YELLOW,
  BACKGROUND_BLUE,
  BACKGROUND_MAGENTA,
  BACKGROUND_CYAN,
  BACKGROUND_WHITE,
  BACKGROUND_DEFAULT = 49,
};

enum Mode { MODE_NORMAL, MODE_INSERT, MODE_COMMAND };
enum RemoveResult { REMOVE_NOTHING, REMOVE_CHAR, REMOVE_LINE };
enum StatusType { STATUS_INFO, STATUS_WARNING, STATUS_ERROR };
enum Command { COMMAND_QUIT, COMMAND_SAVE, COMMAND_OPEN, COMMAND_UNKNOWN };

struct UI {
  bool is_line_numbers;
  bool is_statusline;
  bool is_tabmenu;
};

struct Line {
  char *buf;
  size_t len, size;
};

struct Cell {
  char ch;
  enum RenderMode mode;
  enum ForegroundColor fg;
  enum BackgroundColor bg;
};

struct Status {
  char *msg;
  enum StatusType type;
};

struct Document {
  int x, y;
  int offsetX, offsetY;
  size_t len, size;
  char *path;
  struct Line **buf;
};

struct Context {
  bool is_exit;
  size_t len, size, curr_doc;
  float last_frame;
  char *shortcut;
  struct Document **docs;
  struct Cell **prev_frame;
  struct Cell **curr_frame;
  struct Line *cmd;
  struct Status *status;
  struct termios conf;
  struct termios backup;
  struct winsize win;
  struct UI ui;
  enum Mode mode;
};

#endif
