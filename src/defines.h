#ifndef DEFINES_INCLUDED
#define DEFINES_INCLUDED

#define ADDITIONAL_REALLOCATION 16
#define MAX_BUFFER_SIZE         1024
#define KEY_ENTER               13
#define KEY_BACKSPACE           127
#define KEY_ESCAPE              27
#define KEY_TAB                 9
#define ANSI_MOVE_CURSOR_UP     write(STDOUT_FILENO, "\x1b[1A", 4);
#define ANSI_MOVE_CURSOR_DOWN   write(STDOUT_FILENO, "\x1b[1B", 4);
#define ANSI_MOVE_CURSOR_RIGHT  write(STDOUT_FILENO, "\x1b[1C", 4);
#define ANSI_MOVE_CURSOR_LEFT   write(STDOUT_FILENO, "\x1b[1D", 4);
#define ANSI_RESET_SCREEN       write(STDOUT_FILENO, "\x1b[2J", 4);
#define ANSI_RESET_LINE         write(STDOUT_FILENO, "\x1b[2K", 4);
#define ANSI_MOVE_CURSOR_YX     "\x1b[%d;%dH"
#define ANSI_CURSOR_TYPE        "\x1b[%d q"
#define MIN(A, B)               ((A) > (B) ? (B) : (A))

#endif
