#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"

// TODO: починить
void get_real_path(const char *path, size_t len, char *out, size_t max_size) {
  char *stack[MAX_BUFFER_SIZE];
  int height = sizeof(stack) / sizeof(stack[0]);
  int top = 0;
  for (int i = 0; i < len; i++) {
    char *buf = (char *)xmalloc(MAX_BUFFER_SIZE);
    int curr = 0;
    int j = i;
    while (curr < MAX_BUFFER_SIZE - 1 && j < len && path[j] != '/') {
      buf[curr++] = path[j++];
    }
    buf[curr] = '\0';
    if (i == j || !strcmp(buf, ".")) {
      free(buf);
    } else if (!strcmp(buf, "..")) {
      if (top) top--;
      free(buf);
    } else {
      if (top < height) stack[top++] = buf;
    }
  }
  int curr = 0;
  while (top) {
    if (curr) {
      curr += snprintf(out, max_size - 1, "%s", stack[--top]);
    } else {
      curr += snprintf(out, max_size - 1, "%s%c%s", out, PATH_SEPARATOR, stack[--top]);
    }
    free(stack[top]);
  }
  out[curr] = '\0';
}

const char *get_file_name(char *path) {
  if (!path || path[0] == '\0') return "New buffer";
  char *slash = strrchr(path, '/');
  if (!slash) return path;
  return slash + 1;
}
