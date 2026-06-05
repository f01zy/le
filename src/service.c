#include <ctype.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef WIN32
#include "windows.h"
#else
#include <sys/ioctl.h>
#endif

#include "service.h"

void init_editor(struct Context *ctx) {
  ANSI_RESET_SCREEN;
  tcgetattr(STDIN_FILENO, &ctx->terminal.backup);
  init_context(ctx);
  tcsetattr(STDIN_FILENO, TCSANOW, &ctx->terminal.conf);
  set_editor_mode(ctx, EDITOR_MODE_NORMAL);
  set_statusline_mode(ctx, STATUS_MODE_NORMAL);
}

void quit_editor(struct Context *ctx) {
  free_resources(ctx);
  move_cursor_yx(0, 0);
  ANSI_RESET_SCREEN;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &ctx->terminal.backup);
}

void init_context(struct Context *ctx) {
  gettimeofday(&ctx->frame.prev_time, NULL);
  struct UI ui = {
      .is_line_numbers = true,
      .is_relative_line_numbers = false,
      .is_statusline = true,
      .is_tabmenu = true,
      .is_code_highlighting = true,
  };
  ctx->terminal.size = get_teminal_size();
  ctx->frame.curr = create_frame(ctx);
  ctx->frame.prev = create_frame(ctx);
  ctx->ui = ui;
  ctx->terminal.conf = ctx->terminal.backup;
  ctx->terminal.conf.c_iflag |= IXOFF;
  ctx->terminal.conf.c_iflag &= ~ICRNL;
  ctx->terminal.conf.c_lflag &= ~ECHO;
  ctx->terminal.conf.c_lflag &= ~ISIG;
  ctx->terminal.conf.c_lflag &= ~ICANON;
}

void clear_cmd(struct Context *ctx) {
  ctx->status.cmd.buf[0] = '\0';
  ctx->status.cmd.len = 0;
}

void free_mappings(struct Context *ctx, struct MappingNode *node) {
  for (int i = 0; i < node->len; i++) {
    free_mappings(ctx, node->nodes[i]);
  }
  free(node->nodes);
  free(node);
}

void free_resources(struct Context *ctx) {
  struct Cell **prev_frame = ctx->frame.prev, **curr_frame = ctx->frame.curr;
  for (int i = 0; i < ctx->len; i++) {
    struct Document *doc = ctx->docs[i];
    for (int j = 0; j < doc->len; j++) {
      struct Line *line = doc->buf[j];
      free(line->buf);
      free(line);
    }
    free_tokens(doc);
    free(doc->path);
    free(doc);
  }
  for (int i = 0; i < ctx->terminal.size.y; i++) {
    free(prev_frame[i]);
    free(curr_frame[i]);
  }
  free(ctx->docs);
  free(prev_frame);
  free(curr_frame);
}

void check_offset(struct Context *ctx, struct Document *doc) {
  int width = get_buffer_width(ctx);
  int height = get_buffer_height(ctx);

  if (doc->pos.x < doc->offset.x) {
    doc->offset.x = doc->pos.x;
  } else if (doc->pos.x >= doc->offset.x + width) {
    doc->offset.x = doc->pos.x - width + 1;
  }

  if (doc->pos.y < doc->offset.y) {
    doc->offset.y = doc->pos.y;
  } else if (doc->pos.y >= doc->offset.y + height) {
    doc->offset.y = doc->pos.y - height + 1;
  }
}

void set_editor_mode(struct Context *ctx, enum EditorMode mode) {
  enum CursorStyle style;
  if (mode == EDITOR_MODE_NORMAL) style = CURSOR_BLOCK_BLINKING;
  if (mode == EDITOR_MODE_INSERT) style = CURSOR_LINE_STATIC;
  if (mode == EDITOR_MODE_COMMAND) style = CURSOR_LINE_STATIC;
  ctx->mode = mode;
  set_cursor_style(style);
}

struct Cell **create_frame(struct Context *ctx) {
  int col = ctx->terminal.size.x, row = ctx->terminal.size.y;
  struct Cell **frame = (struct Cell **)xmalloc(row * sizeof(struct Cell *));
  for (int i = 0; i < row; i++) {
    frame[i] = (struct Cell *)xcalloc(col, sizeof(struct Cell));
  }
  return frame;
}

int getchar_nonblock(int ms) {
  struct pollfd pfd;
  pfd.fd = STDIN_FILENO;
  pfd.events = POLLIN;
  int ret = poll(&pfd, 1, ms);
  if (ret > 0 && (pfd.events & POLLIN)) return getchar();
  return -1;
}

void reset_curr_mapping(struct Context *ctx) {
  ctx->mapping.buf[0] = '\0';
  ctx->mapping.len = 0;
}

void set_statusline_message(struct Context *ctx, const char *msg, enum MessageLevel level) {
  set_statusline_mode(ctx, STATUS_MODE_MESSAGE);
  size_t len = MIN(strlen(msg), sizeof(ctx->status.msg.buf) - 1);
  memcpy(ctx->status.msg.buf, msg, len);
  ctx->status.msg.buf[len] = '\0';
  ctx->status.msg.level = level;
}

void set_statusline_dialog(struct Context *ctx, const char *question, void (*on_confirm)(struct Context *), void (*on_deny)(struct Context *)) {
  set_editor_mode(ctx, EDITOR_MODE_DIALOG);
  set_statusline_mode(ctx, STATUS_MODE_DIALOG);
  size_t len = MIN(strlen(question), sizeof(ctx->status.dialog.buf) - 1);
  memcpy(ctx->status.dialog.buf, question, len);
  ctx->status.dialog.buf[len] = '\0';
  ctx->status.dialog.on_confirm = on_confirm;
  ctx->status.dialog.on_deny = on_deny;
}

void unsaved_changes_dialog(struct Context *ctx, void (*on_confirm)(struct Context *ctx)) {
  set_statusline_dialog(ctx, "You have unsaved changes. Are you sure (Y/N): ", on_confirm, NULL);
}

enum ParsingStatus parse_static_mapping(struct Context *ctx, struct MappingNode *ans) {
  struct MappingNode *curr = ctx->mapping.head;
  for (int i = 0; i < ctx->mapping.len; i++) {
    char ch = ctx->mapping.buf[i];
    bool is_found = false;
    for (int j = 0; j < curr->len; j++) {
      if (curr->nodes[j]->ch == ch) {
        curr = curr->nodes[j];
        is_found = true;
        break;
      }
    }
    if (!is_found) return PARSING_STATUS_ERROR;
  }
  *ans = *curr;
  if (curr->len) return PARSING_STATUS_WAITING;
  return PARSING_STATUS_SUCCESS;
}

static const char *motion_operators = "hjklwWeEbB";
static const char *modifier_operators = "dyc";

enum ParsingStatus parse_dinamic_mapping(struct DinamicMapping *mapping, char *buf, size_t len) {
  enum ParseDinamicMappingState state = STATE_MAPPING_GLOBAL_COUNT;
  size_t curr = 0;
  char temp[MAX_STRING_BUFFER_SIZE];

  for (int i = 0; i < len; i++) {
    char ch = buf[i];
    if (curr >= sizeof(temp)) break;
    switch (state) {
    case STATE_MAPPING_GLOBAL_COUNT:
    case STATE_MAPPING_OPERATOR_COUNT:
      if (isdigit(ch)) {
        temp[curr++] = ch;
        break;
      }
      int64_t count = MAX(string_to_number(temp, curr), 1);
      if (state == STATE_MAPPING_GLOBAL_COUNT) {
        mapping->global_count = count;
        state = STATE_MAPPING_OPERATOR;
      } else {
        mapping->op_count = count;
        state = STATE_MAPPING_MODIFIER;
      }
      curr = 0;
      i--;
      break;
    case STATE_MAPPING_OPERATOR:
      mapping->op = ch;
      for (int i = 0; i < strlen(motion_operators); i++) {
        if (motion_operators[i] == ch) return PARSING_STATUS_SUCCESS;
      }
      bool is_modifier_operator = false;
      for (int i = 0; i < strlen(motion_operators); i++) {
        if (motion_operators[i] == ch) {
          is_modifier_operator = true;
          break;
        }
      }
      state = is_modifier_operator ? STATE_MAPPING_MODIFIER : STATE_MAPPING_MOTION_OBJECT;
      break;
    case STATE_MAPPING_MODIFIER:
      if (ch == 'i' || ch == 'a') {
        mapping->modifier = ch;
      } else {
        i--;
      }
      state = STATE_MAPPING_MOTION_OBJECT;
      break;
    case STATE_MAPPING_MOTION_OBJECT:
      mapping->motion_object = ch;
      return PARSING_STATUS_SUCCESS;
    }
  }

  return PARSING_STATUS_ERROR;
}

struct Vec4 get_motion_object_bounds(struct Document *doc, struct DinamicMapping mapping) {
  switch (mapping.motion_object) {
  case 'd':
    if (mapping.op == 'd') return (struct Vec4){0, doc->pos.y, doc->buf[doc->pos.y]->len - 1, doc->pos.y};
    return (struct Vec4){-1};

  default:
    return (struct Vec4){-1};
  }
}

void copy_to_clipboard(const char *data) {
  if (!data) return;
#ifdef WIN32
  size_t len = strlen(data);
  HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
  memcpy(GlobalLock(hMem), data, len);
  GlobalUnlock(hMem);
  OpenClipboard(0);
  EmptyClipboard();
  SetClipboardData(CF_TEXT, hMem);
  CloseClipboard();
#else
  FILE *pipe = popen("xclip -selection clipboard", "w");
  if (pipe) {
    fprintf(pipe, "%s", data);
    pclose(pipe);
  }
#endif
}

struct Vec2 get_teminal_size() {
#ifdef WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  int ret;
  ret = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  if (ret) return (struct Vec2){csbi.dwSize.X, csbi.dwSize.Y};
  exit(1);
#else
  struct winsize size;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
  return (struct Vec2){size.ws_col, size.ws_row};
#endif
}

int64_t string_to_number(const char *buf, size_t len) {
  if (!buf || !len) return -1;
  int64_t ans = 0;
  for (int i = 0; i < len; i++) {
    if (!isdigit(buf[i])) return -1;
    ans = ans * 10 + buf[i] - '0';
  }
  return ans;
}

void set_statusline_mode(struct Context *ctx, enum StatusMode mode) { ctx->status.mode = mode; }
void init_tokens(struct Document *doc) { doc->tokens = scan_tokens(doc); }
void set_flag_to_quit(struct Context *ctx) { ctx->is_need_quit = true; }
