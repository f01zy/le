#include <stdio.h>
#include <string.h>

#include "filesystem.h"
#include "memory.h"

struct GetDocDataRes get_doc_data(const char *path) {
  FILE *file = fopen(path, "r");
  if (!file) return (struct GetDocDataRes){NULL, 0};
  char buf[MAX_BUFFER_SIZE];
  size_t lines = 0;
  struct Line **data = NULL;
  while (fgets(buf, sizeof(buf), file)) {
    buf[strcspn(buf, "\n")] = '\0';
    size_t len = strlen(buf);
    struct Line *line = (struct Line *)xcalloc(1, sizeof(struct Line));
    line->size = len + 1;
    line->len = len;
    line->buf = (char *)xmalloc(len + 1);
    line->buf[len] = '\0';
    memcpy(line->buf, buf, len);
    lines++;
    data = (struct Line **)xrealloc(data, lines * sizeof(struct Line *));
    data[lines - 1] = line;
  }
  fclose(file);
  return (struct GetDocDataRes){data, lines};
}

size_t save_doc_data(const char *path, struct Line **data, size_t len) {
  FILE *file = fopen(path, "w");
  if (!file) return 0;
  size_t written = 0;
  char buf[MAX_BUFFER_SIZE];
  for (int i = 0; i < len; i++) {
    struct Line *line = data[i];
    size_t len = snprintf(buf, sizeof(buf), "%s\n", line->buf);
    fwrite(buf, len, 1, file);
    written += len;
  }
  fclose(file);
  return written;
}
