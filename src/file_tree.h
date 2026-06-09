#ifndef FILE_TREE_INCLUDED
#define FILE_TREE_INCLUDED

#include "types.h"

void free_file_tree(struct FileTreeEntity *ent);
void init_file_tree(struct Context *ctx);
void init_file_tree_labels(struct Context *ctx);

#endif
