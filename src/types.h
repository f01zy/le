#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

#include <stdbool.h>
#include <sys/time.h>
#include <termios.h>

#include "defines.h"

struct Context;

enum StatusMode {
  STATUS_MODE_NORMAL,
  STATUS_MODE_COMMAND,
  STATUS_MODE_MESSAGE,
  STATUS_MODE_DIALOG,
};

enum MessageLevel {
  MESSAGE_INFO,
  MESSAGE_WARNING,
  MESSAGE_ERROR,
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

enum EditorMode {
  EDITOR_MODE_NORMAL,
  EDITOR_MODE_INSERT,
  EDITOR_MODE_COMMAND,
  EDITOR_MODE_DIALOG,
  EDITOR_MODE_VISUAL,
};

struct Vec2 {
  int x, y;
};

struct Vec3 {
  int x, y, z;
};

struct Vec4 {
  int ax, ay;
  int bx, by;
};

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
  struct TokenLine **buf;
  size_t len;
};

struct Line {
  char *buf;
  size_t len, size;
};

struct Document {
  char *path;
  bool is_changed;
  size_t len, size;
  struct Vec3 pos;
  struct Vec2 offset;
  struct Vec2 selected;
  struct Line **buf;
  struct Tokens tokens;
};

struct UI {
  bool is_line_numbers;
  bool is_relative_line_numbers;
  bool is_statusline;
  bool is_tabmenu;
  bool is_code_highlighting;
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
    size_t len;
  } cmd;
};

struct MappingNode {
  int ch;
  const char *desc;
  void (*act)(struct Context *ctx);
  struct MappingNode **nodes;
  size_t len;
};

struct DinamicMapping {
  size_t global_count;
  char op;
  size_t op_count;
  char modifier;
  char motion_object;
};

struct Context {
  bool is_need_quit;
  size_t len, size, curr_doc;
  struct Document **docs;
  struct StatusLine status;
  struct UI ui;
  enum EditorMode mode;

  struct {
    struct termios conf;
    struct termios backup;
    struct Vec2 size;
  } terminal;

  struct {
    struct Cell **prev;
    struct Cell **curr;
    struct timeval prev_time;
  } frame;

  struct {
    struct MappingNode *head;
    char buf[MAX_STRING_BUFFER_SIZE];
    size_t len;
  } mapping;
};

#endif
