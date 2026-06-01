#ifndef MAPPINGS_INCLUDED
#define MAPPINGS_INCLUDED

#include "service.h"

void mapping_up(struct Context *ctx);
void mapping_down(struct Context *ctx);
void mapping_left(struct Context *ctx);
void mapping_right(struct Context *ctx);
void mapping_command_mode(struct Context *ctx);
void mapping_insert_mode_prev(struct Context *ctx);
void mapping_insert_mode_next(struct Context *ctx);
void mapping_line_start(struct Context *ctx);
void mapping_line_end(struct Context *ctx);

void add_mapping_node(struct Context *ctx, struct MappingNode *head, struct Mapping map);
void init_mappings(struct Context *ctx);

#endif
