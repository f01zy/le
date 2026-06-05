#ifndef SERVICE_INCLUDED
#define SERVICE_INCLUDED

#include <stdint.h>

#include "lexer.h"
#include "terminal.h"

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

void set_statusline_message(struct Context *ctx, const char *msg, enum MessageLevel level);
void set_statusline_dialog(struct Context *ctx, const char *question, void (*on_confirm)(struct Context *ctx), void (*on_deny)(struct Context *ctx));
void set_editor_mode(struct Context *ctx, enum EditorMode mode);
void set_statusline_mode(struct Context *ctx, enum StatusMode mode);

void init_tokens(struct Document *doc);
void init_context(struct Context *ctx);
void init_editor(struct Context *ctx);
void set_flag_to_quit(struct Context *ctx);
void quit_editor(struct Context *ctx);

void free_mappings(struct MappingNode *node);
void free_resources(struct Context *ctx);

enum ParsingStatus parse_dinamic_mapping(struct Context *ctx, struct DinamicMapping *ans);
enum ParsingStatus parse_static_mapping(struct Context *ctx, struct MappingNode *ans);
void reset_curr_mapping(struct Context *ctx);

struct Vec2 get_teminal_size();
struct Cell **create_frame(struct Vec2 size);
int64_t string_to_number(const char *buf, size_t len);
void copy_to_clipboard(const char *data);
void clear_cmd(struct Context *ctx);
void unsaved_changes_dialog(struct Context *ctx, void (*on_confirm)(struct Context *ctx));
void check_offset(struct Document *doc, struct UI ui, struct Vec2 size);

int getchar_nonblock(int ms);

#endif
