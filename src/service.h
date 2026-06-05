#ifndef SERVICE_INCLUDED
#define SERVICE_INCLUDED

#include <stdint.h>

#include "filesystem.h"
#include "render.h"

enum ParseDinamicMappingState {
  STATE_MAPPING_GLOBAL_COUNT,
  STATE_MAPPING_OPERATOR,
  STATE_MAPPING_OPERATOR_COUNT,
  STATE_MAPPING_MODIFIER,
  STATE_MAPPING_MOTION_OBJECT,
};

enum ParsingStatus {
  PARSING_STATUS_SUCCESS,
  PARSING_STATUS_WAITING,
  PARSING_STATUS_ERROR,
};

struct DinamicMapping {
  enum ParsingStatus status;
  size_t global_count;
  char op;
  size_t op_count;
  char modifier;
  char motion_object;
};

void set_statusline_message(struct Context *ctx, const char *msg, enum MessageLevel level);
void set_statusline_dialog(struct Context *ctx, const char *question, void (*on_confirm)(struct Context *ctx), void (*on_deny)(struct Context *ctx));
void set_editor_mode(struct Context *ctx, enum EditorMode mode);
void set_statusline_mode(struct Context *ctx, enum StatusMode mode);

void init_tokens(struct Document *doc);
void init_context(struct Context *ctx);
void init_editor(struct Context *ctx);
void set_flag_to_quit(struct Context *ctx);
void quit_editor(struct Context *ctx);

void free_mappings(struct Context *ctx, struct MappingNode *node);
void free_resources(struct Context *ctx);

struct Vec4 get_motion_object_bounds(struct Document *doc, struct DinamicMapping mapping);
enum ParsingStatus parse_dinamic_mapping(struct DinamicMapping *mapping, char *buf, size_t len);
enum ParsingStatus parse_static_mapping(struct Context *ctx, struct MappingNode *ans);
void reset_curr_mapping(struct Context *ctx);

struct Cell **create_frame(struct Context *ctx);
int64_t string_to_number(const char *buf, size_t len);
void copy_to_clipboard(const char *data);
void clear_cmd(struct Context *ctx);
void unsaved_changes_dialog(struct Context *ctx, void (*on_confirm)(struct Context *ctx));
void check_offset(struct Context *ctx, struct Document *doc);

int getchar_nonblock(int ms);

#endif
