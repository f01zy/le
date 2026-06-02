#include "lexer.h"
#include <stdlib.h>

struct Tokens scan_tokens(struct Document *doc) {
  free_tokens(doc);
  struct TokenLine **lines = (struct TokenLine **)xmalloc(doc->len * sizeof(struct TokenLine *));
  for (int i = 0; i < doc->len; i++) {
    lines[i] = (struct TokenLine *)xcalloc(1, sizeof(struct TokenLine));
  }
  struct Lexer lexer = {0, 0, 0, doc->len, lines, doc};
  while (!is_at_end(&lexer)) {
    lexer.start = lexer.curr;
    scan_token(&lexer);
  }
  return (struct Tokens){lexer.lines, lexer.len};
}

void scan_token(struct Lexer *lexer) {
  char ch = advance(lexer);
  switch (ch) {
  case '"':
    token_string(lexer);
    break;
  }
}

char advance(struct Lexer *lexer) {
  struct Line *line = lexer->doc->buf[lexer->line];
  if (lexer->curr >= line->len) {
    lexer->curr = -1;
    lexer->line++;
    line = lexer->doc->buf[lexer->line];
  }
  return line->buf[lexer->curr++];
}

void add_token(struct Lexer *lexer, enum TokenType type) {
  struct TokenLine *line = lexer->lines[lexer->line];
  line->len++;
  if (line->len >= line->size) {
    line->size = line->len + ADDITIONAL_REALLOCATION;
    line->buf = (struct Token *)xrealloc(line->buf, line->size * sizeof(struct Token));
  }
  struct Token *token = &line->buf[line->len - 1];
  token->type = type;
  token->start = lexer->start;
  token->len = lexer->curr - lexer->start;
}

bool is_at_end(struct Lexer *lexer) { return lexer->curr >= lexer->doc->buf[lexer->doc->len - 1]->len && lexer->line >= lexer->doc->len - 1; }

char peek(struct Lexer *lexer) {
  if (is_at_end(lexer)) return '\0';
  struct Line *line = lexer->doc->buf[lexer->line];
  return line->buf[lexer->curr];
}

void token_string(struct Lexer *lexer) {
  while (peek(lexer) != '"' && !is_at_end(lexer)) {
    advance(lexer);
  }
  if (peek(lexer) == '\0') return;
  advance(lexer);
  add_token(lexer, LITERAL_STRING);
}

void free_tokens(struct Document *doc) {
  if (doc->tokens.lines) {
    for (int i = 0; i < doc->tokens.len; i++) {
      free(doc->tokens.lines[i]);
    }
    free(doc->tokens.lines);
  }
  doc->tokens = (struct Tokens){NULL, 0};
}
