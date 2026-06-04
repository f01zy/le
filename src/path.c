#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "path.h"

void get_real_path(const char *path, size_t len, char *out, size_t max_size) {
  char *stack[MAX_BUFFER_SIZE];
  int height = sizeof(stack) / sizeof(stack[0]);
  int top = 0;
  for (int i = 0; i < len; i++) {
    char *buf = (char *)xmalloc(MAX_BUFFER_SIZE);
    int curr = 0, j = i;
    while (curr < MAX_BUFFER_SIZE - 1 && j < len && path[j] != '/') {
      buf[curr++] = path[j++];
    }
    buf[curr] = '\0';
    if (i == j) {
      free(buf);
      continue;
    }
    i = j;
    if (!strcmp(buf, ".")) {
      free(buf);
    } else if (!strcmp(buf, "..")) {
      if (top) free(stack[--top]);
      free(buf);
    } else {
      if (top < height) stack[top++] = buf;
    }
  }
  // TODO: доделать под винду
  int curr = 0;
  for (int i = 0; i < top; i++) {
    char temp[MAX_BUFFER_SIZE];
    size_t count = snprintf(temp, sizeof(temp), "%c%s", PATH_SEPARATOR, stack[i]);
    xmemcpy(out + curr, max_size - curr - 1, temp, count);
    curr += count;
    free(stack[i]);
  }
  out[curr] = '\0';
}

const char *get_file_name(char *path) {
  if (!path || path[0] == '\0') return "New buffer";
  char *slash = strrchr(path, '/');
  if (!slash) return path;
  return slash + 1;
}
