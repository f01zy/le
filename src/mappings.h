#ifndef MAPPINGS_INCLUDED
#define MAPPINGS_INCLUDED

#include "service.h"

void cmd_up(struct Context *ctx);
void cmd_down(struct Context *ctx);
void cmd_left(struct Context *ctx);
void cmd_right(struct Context *ctx);
void cmd_line_start(struct Context *ctx);
void cmd_line_end(struct Context *ctx);
void cmd_doc_start(struct Context *ctx);
void cmd_doc_end(struct Context *ctx);
void cmd_doc_prev(struct Context *ctx);
void cmd_doc_next(struct Context *ctx);
void cmd_doc_new(struct Context *ctx);
void _cmd_doc_close(struct Context *ctx);
void cmd_doc_close(struct Context *ctx);
void cmd_command_mode(struct Context *ctx);
void cmd_insert_mode_prev(struct Context *ctx);
void cmd_insert_mode_next(struct Context *ctx);
void cmd_toggle_line_numbers(struct Context *ctx);

void add_mapping_node(struct Context *ctx, struct MappingNode *head, struct Mapping map);
void init_mappings(struct Context *ctx);

#endif
