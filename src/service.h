#ifndef SERVICE_INCLUDED
#define SERVICE_INCLUDED

#include "text_objects.h"
#include <stdint.h>

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
void init_editor(struct Context *ctx);
void set_flag_to_quit(struct Context *ctx);
void quit_editor(struct Context *ctx);

void free_resources(struct Context *ctx);

enum ParsingStatus parse_dinamic_mapping(struct Context *ctx, struct DinamicMapping *ans);
enum ParsingStatus parse_static_mapping(struct Context *ctx, struct MappingNode *ans);
void reset_curr_mapping(struct Context *ctx);

void copy_to_clipboard(const char *data);
void clear_cmd(struct Context *ctx);
void unsaved_changes_dialog(struct Context *ctx, void (*on_confirm)(struct Context *ctx));

void update_tree_offset(struct Context *ctx);
void update_doc_offset(struct Document *doc, struct UI ui, struct Vec2 size);
void update_doc_max_x(struct Document *doc);

bool is_sticky_motion(char ch);
int getchar_nonblock(int ms);

#endif
