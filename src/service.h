#ifndef SERVICE_INCLUDED
#define SERVICE_INCLUDED

#include "buffer.h"
#include "lexer.h"
#include "terminal.h"
#include "ui.h"

void set_statusline_message(struct Context *ctx, const char *msg, enum MessageLevel level);
void set_statusline_dialog(struct Context *ctx, const char *question, void (*on_confirm)(struct Context *ctx), void (*on_deny)(struct Context *ctx));
void set_editor_mode(struct Context *ctx, enum EditorMode mode);
void set_statusline_mode(struct Context *ctx, enum StatusMode mode);

void init_highlightings(struct Document *doc);
void init_context(struct Context *ctx);
void init_editor(struct Context *ctx);
void set_flag_to_quit(struct Context *ctx);
void quit_editor(struct Context *ctx);

void copy_to_clipboard(const char *data);
struct Cell **create_frame(struct Context *ctx);
void reset_curr_mapping(struct Context *ctx);
void exec_curr_mapping(struct Context *ctx);
void clear_cmd(struct Context *ctx);
void unsaved_changes_dialog(struct Context *ctx, void (*on_confirm)(struct Context *ctx));
void free_mappings(struct Context *ctx, struct MappingNode *node);
void free_resources(struct Context *ctx);
void check_offset(struct Context *ctx, struct Document *doc);
int getchar_nonblock(int ms);
const char *get_file_name(char *path);
const char *get_editor_mode_label(struct Context *ctx);
enum ForegroundColor get_token_foreground(enum TokenGroup group);

#endif
