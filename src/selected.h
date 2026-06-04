#ifndef SELECTED_INCLUDED
#define SELECTED_INCLUDED

#include "memory.h"
#include "types.h"

void get_selected_coordinates(struct Vec4 *c);
void get_selected_buffer(struct Document *doc, char *out, size_t max_size);

#endif
