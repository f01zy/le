#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>

#include "defines.h"

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

enum TokenGroup {
  TOKEN_LITERAL_STRING,
  TOKEN_LITERAL_NUMBER,
  TOKEN_LITERAL_SYMBOL,
  TOKEN_KEYWORD,
  TOKEN_DATA_TYPE,
  TOKEN_FUNCTION,
  TOKEN_DELIMITER,
  TOKEN_DIRECTIVE,
};

enum EditorMode { EDITOR_MODE_NORMAL, EDITOR_MODE_INSERT, EDITOR_MODE_COMMAND, EDITOR_MODE_DIALOG };
enum StatusMode { STATUS_MODE_NORMAL, STATUS_MODE_COMMAND, STATUS_MODE_MESSAGE, STATUS_MODE_DIALOG };
enum RemoveResult { REMOVE_NOTHING, REMOVE_CHAR, REMOVE_LINE };
enum MessageLevel { MESSAGE_INFO, MESSAGE_WARNING, MESSAGE_ERROR };

struct Context;

struct Token {
  enum TokenGroup group;
  size_t start, len;
};

struct TokenLine {
  struct Token *buf;
  size_t len, size;
};

struct Lexer {
  int start;
  int curr;
  int line;
  size_t len;
  struct TokenLine **lines;
  struct Document *doc;
};

struct Tokens {
  struct TokenLine **lines;
  size_t len;
};

struct UI {
  bool is_line_numbers;
  bool is_statusline;
  bool is_tabmenu;
  bool is_mappings_menu;
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

struct Document {
  int x, y;
  int offsetX, offsetY;
  char *path;
  bool is_changed;
  size_t len, size;
  struct Line **buf;
  struct Tokens tokens;
};

struct StatusLine {
  enum StatusMode mode;

  struct {
    char buf[MAX_STRING_BUFFER_SIZE];
    enum MessageLevel level;
  } msg;

  struct {
    char buf[MAX_STRING_BUFFER_SIZE];
    void (*on_confirm)(struct Context *ctx);
    void (*on_deny)(struct Context *ctx);
  } dialog;

  struct {
    char buf[MAX_STRING_BUFFER_SIZE];
    int len;
  } cmd;
};

struct Context {
  bool is_need_quit;
  size_t len, size, curr_doc;
  struct Document **docs;
  struct MappingNode *curr_mapping;
  struct MappingNode *head_mapping;
  struct Cell **prev_frame;
  struct Cell **curr_frame;
  struct termios conf;
  struct termios backup;
  struct winsize win;
  struct timeval prev_frame_time;
  struct StatusLine status;
  struct UI ui;
  enum EditorMode mode;
};

struct MappingNode {
  int ch;
  const char *desc;
  void (*act)(struct Context *ctx);
  struct MappingNode **nodes;
  size_t len;
};

struct Mapping {
  const char *cmd;
  const char *desc;
  void (*act)(struct Context *ctx);
};

struct Command {
  const char *cmd;
  void (*act)(struct Context *ctx, char *token);
};

#endif
