#ifndef BUFFER_INCLUDED
#define BUFFER_INCLUDED

#include "types.h"

enum RemoveResult {
  REMOVE_NOTHING,
  REMOVE_CHAR,
  REMOVE_LINE,
};

void set_doc_path(struct Document *doc, const char *path);
void remove_doc_path(struct Document *doc);

struct Document *create_doc(struct Context *ctx);
bool init_doc(struct Document *doc, const char *path);
size_t save_doc(struct Document *doc);
void add_line(struct Document *doc, char *data, int y);
void remove_line(struct Document *doc, int y);
void write_to_line(struct Document *doc, int y, int x, char ch);
void remove_range(struct Document *doc, struct Vec4 c);
enum RemoveResult remove_from_line(struct Document *doc, int y, int x);
void line_break(struct Document *doc);

#endif
