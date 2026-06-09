// TODO: использовать windows.h для рендера на винде

#include <stdio.h>
#include <sys/ioctl.h>

#include "terminal.h"

void move_cursor_yx(int y, int x) {
  char buf[MAX_BUFFER_SIZE];
  size_t len = snprintf(buf, sizeof(buf), ANSI_MOVE_CURSOR_YX, y + 1, x + 1);
  write(STDOUT_FILENO, buf, len);
}

void set_cursor_style(enum CursorStyle type) {
  char buf[MAX_BUFFER_SIZE];
  size_t len = snprintf(buf, sizeof(buf), ANSI_CURSOR_TYPE, type);
  write(STDOUT_FILENO, buf, len);
}

void set_render_mode(enum RenderMode mode, enum ForegroundColor fg, enum BackgroundColor bg) {
  char buf[MAX_BUFFER_SIZE];
  size_t len;
  if (bg == BACKGROUND_DEFAULT) {
    len = snprintf(buf, sizeof(buf), ANSI_RENDER_MODE ANSI_FOREGROUND ANSI_BACKGROUND_DEFAULT, mode, fg);
  } else {
    len = snprintf(buf, sizeof(buf), ANSI_RENDER_MODE ANSI_FOREGROUND ANSI_BACKGROUND, mode, fg, bg);
  }
  write(STDOUT_FILENO, buf, len);
}

struct Vec2 get_terminal_size() {
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
