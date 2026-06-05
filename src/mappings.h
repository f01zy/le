#ifndef MAPPINGS_INCLUDED
#define MAPPINGS_INCLUDED

#include "service.h"

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
  X(yank, "Yank selected text", "y")                                                                                                                           \
  X(toggle_code_highlighting, "Toggle code highlighting", " h")                                                                                                \
  X(toggle_line_numbers, "Toggle line numbers", " n")

// TODO: вернуть кеймап удаления выделенного текста. щас конфликт режимов редактора

#define X(name, desc, mapping) void cmd_##name(struct Context *ctx);
COMMANDS_LIST
#undef X

void exec_mapping(struct Context *ctx);
void add_mapping_node(struct Context *ctx, struct MappingNode *head, struct Mapping map);
void init_mappings(struct Context *ctx);

#endif
