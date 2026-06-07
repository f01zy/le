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

#define MAPPINGS_LIST                                                                                                                                          \
  X(line_start, "Move to start of line", MAPPING_LINE_START)                                                                                                   \
  X(doc_start, "Move to start of document", MAPPING_DOC_START)                                                                                                 \
  X(doc_end, "Move to end of document", MAPPING_DOC_END)                                                                                                       \
  X(doc_next, "Move to next document", MAPPING_DOC_NEXT)                                                                                                       \
  X(doc_prev, "Move to previous document", MAPPING_DOC_PREV)                                                                                                   \
  X(doc_new, "New document", MAPPING_DOC_NEW)                                                                                                                  \
  X(doc_close, "Close document", MAPPING_DOC_CLOSE)                                                                                                            \
  X(insert_mode_prev, "Enable insert mode", MAPPING_INSERT_MODE_PREV)                                                                                          \
  X(insert_mode_next, "Enable insert mode", MAPPING_INSERT_MODE_NEXT)                                                                                          \
  X(command_mode, "Enable command mode", MAPPING_COMMAND_MODE)                                                                                                 \
  X(visual_mode, "Enable visual mode", MAPPING_VISUAL_MODE)                                                                                                    \
  X(toggle_code_highlighting, "Toggle code highlighting", MAPPING_TOGGLE_CODE_HIGHLIGHTING)                                                                    \
  X(toggle_line_numbers, "Toggle line numbers", MAPPING_TOGGLE_LINE_NUMBERS)                                                                                   \
  X(toggle_relative_line_numbers, "Toggle relative line numbers", MAPPING_TOGGLE_RELATIVE_LINE_NUMBERS)

void exec_mapping(struct Context *ctx);
void init_mappings(struct Context *ctx);

#endif
