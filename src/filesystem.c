#include <stdio.h>
#include <string.h>

#include "filesystem.h"

bool load_doc_data(struct Document *doc, char *path) {
  FILE *file = fopen(path, "r");
  if (!file) return false;
  set_doc_path(doc, path);
  for (int i = doc->len - 1; i >= 0; i--) {
    remove_line(doc, i);
  }
  char buf[MAX_BUFFER_SIZE];
  while (fgets(buf, sizeof(buf), file)) {
    buf[strcspn(buf, "\n")] = '\0';
    add_line(doc, buf, doc->len);
  }
  fclose(file);
  return true;
}

int save_doc(struct Document *doc) {
  char buf[MAX_BUFFER_SIZE];
  FILE *file = fopen(doc->path, "w");
  if (!file) return -1;
  int size = 0;
  for (int i = 0; i < doc->len; i++) {
    struct Line *line = doc->buf[i];
    size_t len = snprintf(buf, sizeof(buf), "%s\n", line->buf);
    fwrite(buf, len, 1, file);
    size += len;
  }
  fclose(file);
  return size;
}
