#ifndef MAPPINGS_INCLUDED
#define MAPPINGS_INCLUDED

#include "buffer.h"
#include "service.h"
#include "text_objects.h"

struct Mapping {
  const char *cmd;
  const char *desc;
  void (*act)(struct Context *ctx);
};

#define COMMANDS_LIST                                                                                                                                          \
  X(toggle_relative_line_numbers, "Toggle relative line numbers", " rn")                                                                                       \
  X(line_start, "Move to start of line", "0")                                                                                                                  \
  X(line_end, "Move to end of line", "$")                                                                                                                      \
  X(doc_start, "Move to start of document", "gg")                                                                                                              \
  X(doc_end, "Move to end of document", "G")                                                                                                                   \
  X(doc_next, "Move to next document", "\t")                                                                                                                   \
  X(doc_prev, "Move to previous document", KEY_SHIFT_TAB)                                                                                                      \
  X(doc_new, "New document", " dn")                                                                                                                            \
  X(doc_close, "Close document", " dc")                                                                                                                        \
  X(insert_mode_prev, "Enable insert mode", "i")                                                                                                               \
  X(insert_mode_next, "Enable insert mode", "a")                                                                                                               \
  X(command_mode, "Enable command mode", ":")                                                                                                                  \
  X(visual_mode, "Enable visual mode", "v")                                                                                                                    \
  X(toggle_code_highlighting, "Toggle code highlighting", " h")                                                                                                \
  X(toggle_line_numbers, "Toggle line numbers", " n")

void exec_mapping(struct Context *ctx);
void init_mappings(struct Context *ctx);

#endif
