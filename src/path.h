#ifndef PATH_INCLUDED
#define PATH_INCLUDED

#include <stddef.h>

void get_real_path(const char *path, size_t len, char *out, size_t max_size);
const char *get_file_name(char *path);

#endif
