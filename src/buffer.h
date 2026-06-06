#ifndef BUFFER_INCLUDED
#define BUFFER_INCLUDED

#include "memory.h"
#include "path.h"
#include "selected.h"

enum RemoveResult {
  REMOVE_NOTHING,
  REMOVE_CHAR,
  REMOVE_LINE,
};

struct Document *create_doc(struct Context *ctx);
void set_doc_path(struct Document *doc, char *path);
void remove_doc_path(struct Document *doc);
void add_line(struct Document *doc, char *data, int y);
void remove_line(struct Document *doc, int y);
void write_to_line(struct Document *doc, int y, int x, char ch);
void remove_range(struct Document *doc, struct Vec4 c);
enum RemoveResult remove_from_line(struct Document *doc, int y, int x);
void line_break(struct Document *doc);

#endif
