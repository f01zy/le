#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

void *xcalloc(size_t n, size_t size) {
  void *ptr = calloc(n, size);
  if (!ptr) {
    perror("Memory allocation failed");
    exit(1);
  }
  return ptr;
}

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (!ptr) {
    perror("Memory allocation failed");
    exit(1);
  }
  return ptr;
}

void *xrealloc(void *ptr, size_t size) {
  void *new_ptr = realloc(ptr, size);
  if (!new_ptr && size > 0) {
    perror("Memory reallocation failed");
    exit(1);
  }
  return new_ptr;
}

void *xmemcpy(void *dest, size_t dest_size, const void *src, size_t n) {
  if (n > dest_size) return NULL;
  return memcpy(dest, src, n);
}
