#ifndef FILESYSTEM_INCLUDED
#define FILESYSTEM_INCLUDED

#include "types.h"

struct GetDocDataRes {
  struct Line **data;
  size_t len;
};

struct GetDocDataRes get_doc_data(const char *path);
size_t save_doc_data(const char *path, struct Line **data, size_t len);

#endif
