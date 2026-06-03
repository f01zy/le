#ifndef DEFINES_INCLUDED
#define DEFINES_INCLUDED

#define ADDITIONAL_REALLOCATION     16
#define MAX_BUFFER_SIZE             8192
#define MAPPINGS_COL                5
#define MAPPINGS_COL_WIDTH          40
#define MAX_STRING_BUFFER_SIZE      256
#define KEY_ENTER                   13
#define KEY_BACKSPACE               127
#define KEY_ESCAPE                  27
#define KEY_TAB                     9
#define KEY_SHIFT_TAB               "\x1b[Z"
#define ANSI_RESET_SCREEN           write(STDOUT_FILENO, "\x1b[2J", 4)
#define ANSI_RESET_LINE_FROM_CURSOR write(STDOUT_FILENO, "\x1b[0K", 4)
#define ANSI_RESET_LINE_TO_CURSOR   write(STDOUT_FILENO, "\x1b[0K", 4)
#define ANSI_MOVE_CURSOR_YX         "\x1b[%d;%dH"
#define ANSI_CURSOR_TYPE            "\x1b[%d q"
#define ANSI_RENDER_MODE            "\x1b[%d;38;5;%d;48;5;%dm"
#define MIN(A, B)                   ((A) < (B) ? (A) : (B))
#define MAX(A, B)                   ((A) > (B) ? (A) : (B))
#define CELL(ch)                    ((struct Cell){(ch), RENDER_DEFAULT, FOREGROUND_WHITE, BACKGROUND_BLACK})
#define CELL_MODE(ch, mode)         ((struct Cell){(ch), (mode), FOREGROUND_WHITE, BACKGROUND_BLACK})

#endif
