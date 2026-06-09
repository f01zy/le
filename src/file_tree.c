#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "file_tree.h"
#include "memory.h"

size_t file_tree_ents_count(struct FileTreeEntity *ent) {
  if (ent->type == ENTITY_DIRECTORY) {
    size_t children = 0;
    for (int i = 0; i < ent->as.dir.len; i++) {
      children += file_tree_ents_count(ent->as.dir.children[i]);
    }
    return children + 1;
  }
  return 1;
}

struct FileTreeEntity *init_file_tree_ent(const char *path) {
  struct FileTreeEntity *ent = (struct FileTreeEntity *)xcalloc(1, sizeof(struct FileTreeEntity));
  struct stat ent_stat;
  stat(path, &ent_stat);
  size_t path_len = strlen(path);
  ent->path = (char *)xmalloc(path_len + 1);
  ent->path[path_len] = '\0';
  memcpy(ent->path, path, path_len);

  if (S_ISREG(ent_stat.st_mode)) {
    ent->type = ENTITY_FILE;
    ent->as.file.is_readonly = false;
    ent->as.file.size = ent_stat.st_size;
  }

  else if (S_ISDIR(ent_stat.st_mode)) {
    DIR *dir = opendir(path);
    if (!dir) return NULL;
    ent->type = ENTITY_DIRECTORY;
    ent->as.dir.is_open = false;
    struct dirent *child_ent;
    struct FileTreeEntity **children = NULL;
    size_t count = 0;
    while ((child_ent = readdir(dir))) {
      if (!strcmp(child_ent->d_name, ".") || !strcmp(child_ent->d_name, "..")) continue;
      char child_path[MAX_STRING_BUFFER_SIZE];
      size_t len = snprintf(child_path, sizeof(child_path) - 1, "%s/%s", path, child_ent->d_name);
      child_path[len] = '\0';
      struct FileTreeEntity *child = init_file_tree_ent(child_path);
      children = (struct FileTreeEntity **)xrealloc(children, ++count * sizeof(struct FileTreeEntity *));
      children[count - 1] = child;
    }
    ent->as.dir.len = count;
    ent->as.dir.children = children;
  }

  return ent;
}

void init_file_tree(struct Context *ctx) {
  char cwd[MAX_STRING_BUFFER_SIZE];
  if (!get_curr_dir(cwd, sizeof(cwd))) return;
  memcpy(ctx->file_tree.root_dir, cwd, strlen(cwd));
  ctx->file_tree.root = init_file_tree_ent(cwd);
  ctx->file_tree.size = file_tree_ents_count(ctx->file_tree.root);
}

void free_file_tree(struct FileTreeEntity *ent) {
  if (ent->type == ENTITY_DIRECTORY) {
    for (int i = 0; i < ent->as.dir.len; i++) {
      free_file_tree(ent->as.dir.children[i]);
    }
    free(ent->as.dir.children);
  }
  free(ent->path);
  free(ent);
}

void _init_file_tree_labels(struct FileTreeEntity *dir, struct FileTreeLabel *labels, size_t *len, size_t level) {
  if (dir->type != ENTITY_DIRECTORY) return;
  for (int i = 0; i < dir->as.dir.len; i++) {
    struct FileTreeEntity *ent = dir->as.dir.children[i];
    labels[(*len)++] = (struct FileTreeLabel){ent, level};
    if (ent->type == ENTITY_DIRECTORY && ent->as.dir.is_open) _init_file_tree_labels(ent, labels, len, level + 1);
  }
}

void init_file_tree_labels(struct Context *ctx) {
  struct FileTreeEntity *root = ctx->file_tree.root;
  struct FileTreeLabel *labels = (struct FileTreeLabel *)xmalloc(ctx->file_tree.size * sizeof(struct FileTreeLabel));
  size_t len = 0;
  _init_file_tree_labels(root, labels, &len, 0);
  free(ctx->file_tree.labels);
  ctx->file_tree.labels = labels;
  ctx->file_tree.len = len;
}
