#ifndef FILESYSTEM_INCLUDED
#define FILESYSTEM_INCLUDED

#include "buffer.h"

bool load_doc_data(struct Document *doc, char *path);
int save_doc(struct Document *doc);

#endif
