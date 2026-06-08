#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "file_tree.h"
#include "memory.h"

struct FileTreeEntities init_file_tree_folder(const char *path) {
  DIR *dir = opendir(path);
  if (!dir) return (struct FileTreeEntities){NULL, 0};
  struct dirent *ent;
  struct FileTreeEntity **children;
  size_t count = 0;
  while ((ent = readdir(dir))) {
    if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) continue;
    char ent_path[MAX_STRING_BUFFER_SIZE];
    size_t len = snprintf(ent_path, sizeof(ent_path), "%s/%s", path, ent->d_name);
    struct stat ent_stat;
    stat(ent_path, &ent_stat);
    struct FileTreeEntity *res = (struct FileTreeEntity *)xcalloc(1, sizeof(struct FileTreeEntity));
    res->path = (char *)xmalloc(len + 1);
    res->path[len] = '\0';
    memcpy(res->path, ent_path, len);
    if (S_ISDIR(ent_stat.st_mode)) {
      res->type = ENTITY_DIRECTORY;
    } else if (S_ISREG(ent_stat.st_mode)) {
      res->type = ENTITY_FILE;
    }
    children = (struct FileTreeEntity **)xrealloc(children, ++count * sizeof(struct FileTreeEntity *));
    children[count - 1] = res;
  }
  return (struct FileTreeEntities){children, count};
}
