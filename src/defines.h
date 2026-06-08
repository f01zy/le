#ifndef DEFINES_INCLUDED
#define DEFINES_INCLUDED

#include <ctype.h>

#ifdef WIN32
#include <dirent.h>
#define get_curr_dir   _getcwd
#define PATH_SEPARATOR '\\'
#else
#include <unistd.h>
#define get_curr_dir   getcwd
#define PATH_SEPARATOR '/'
#endif

#define ADDITIONAL_REALLOCATION              16
#define MAX_BUFFER_SIZE                      8192
#define MAX_STRING_BUFFER_SIZE               256
#define ALPHABET_MIN                         32
#define ALPHABET_MAX                         126
#define MAPPINGS_COL                         5
#define MAPPINGS_COL_WIDTH                   40
#define FILE_TREE_WIDTH                      30

#define KEY_ENTER                            13
#define KEY_BACKSPACE                        127
#define KEY_ESCAPE                           27
#define KEY_TAB                              9
#define KEY_CTRL_L                           "\x0c"
#define KEY_CTRL_H                           "\x08"
#define KEY_CTRL_N                           "\x0e"

#define ANSI_RESET_SCREEN                    write(STDOUT_FILENO, "\x1b[2J", 4)
#define ANSI_RESET_LINE_FROM_CURSOR          write(STDOUT_FILENO, "\x1b[0K", 4)
#define ANSI_RESET_LINE_TO_CURSOR            write(STDOUT_FILENO, "\x1b[0K", 4)
#define ANSI_HIDE_CURSOR                     write(STDOUT_FILENO, "\x1b[?25l", 6)
#define ANSI_SHOW_CURSOR                     write(STDOUT_FILENO, "\x1b[?25h", 6)
#define ANSI_MOVE_CURSOR_YX                  "\x1b[%d;%dH"
#define ANSI_CURSOR_TYPE                     "\x1b[%d q"
#define ANSI_RENDER_MODE                     "\x1b[%d;38;5;%d;48;5;%dm"

#define MAPPING_LEFT                         'h'
#define MAPPING_RIGHT                        'l'
#define MAPPING_DOWN                         'j'
#define MAPPING_UP                           'k'
#define MAPPING_YANK                         'y'
#define MAPPING_DELETE                       'd'
#define MAPPING_CHANGE                       'c'
#define MAPPING_LINE_END                     '$'
#define MAPPING_LINE_START                   "0"
#define MAPPING_DOC_START                    "gg"
#define MAPPING_DOC_END                      "G"
#define MAPPING_DOC_NEXT                     KEY_CTRL_L
#define MAPPING_DOC_PREV                     KEY_CTRL_H
#define MAPPING_DOC_NEW                      " dn"
#define MAPPING_DOC_CLOSE                    " dc"
#define MAPPING_INSERT_MODE_PREV             "i"
#define MAPPING_INSERT_MODE_NEXT             "a"
#define MAPPING_COMMAND_MODE                 ":"
#define MAPPING_VISUAL_MODE                  "v"
#define MAPPING_TOGGLE_CODE_HIGHLIGHTING     " h"
#define MAPPING_TOGGLE_LINE_NUMBERS          " n"
#define MAPPING_TOGGLE_RELATIVE_LINE_NUMBERS " rn"
#define MAPPING_TOGGLE_FILE_TREE             KEY_CTRL_N

#define MAX_LINE_X(A)                        ((A) ? (A) - 1 : 0)
#define MIN(A, B)                            ((A) < (B) ? (A) : (B))
#define MAX(A, B)                            ((A) > (B) ? (A) : (B))
#define IS_ALPHA(A)                          (isalpha((unsigned char)(A)) || (A) == '_')
#define IS_ALPHA_NUMERIC(A)                  (isalpha((unsigned char)(A)) || (A) == '_' || isdigit(A))
#define CELL(ch)                             ((struct Cell){(ch), RENDER_DEFAULT, FOREGROUND_WHITE, BACKGROUND_BLACK})
#define CELL_MODE(ch, mode)                  ((struct Cell){(ch), (mode), FOREGROUND_WHITE, BACKGROUND_BLACK})

#endif
