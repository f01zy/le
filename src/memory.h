#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED

#include <stddef.h>

void *xcalloc(size_t n, size_t size);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xmemcpy(void *dest, size_t dest_size, const void *src, size_t n);

#endif
