#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static const char *keywords[] = {
    "if", "else", "switch", "case", "default", "while", "do", "for", "break", "continue", "return", "goto", "sizeof", "typedef",
};

static const char *data_types[] = {
    "int",   "char", "float", "double", "void",   "long",     "short",    "signed", "unsigned", "struct",
    "union", "enum", "const", "static", "extern", "volatile", "register", "bool",   "size_t",
};

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

char advance(struct Lexer *lexer) {
  struct Line *line = lexer->doc->buf[lexer->line];
  if (lexer->curr >= line->len) {
    lexer->curr = -1;
    lexer->line++;
    line = lexer->doc->buf[lexer->line];
  }
  return line->buf[lexer->curr++];
}

char peek(struct Lexer *lexer) {
  if (is_at_end(lexer)) return '\0';
  struct Line *line = lexer->doc->buf[lexer->line];
  return line->buf[lexer->curr];
}

char peek_next(struct Lexer *lexer) {
  struct Line *line = lexer->doc->buf[lexer->line];
  if (lexer->curr >= line->len) return '\0';
  return line->buf[lexer->curr + 1];
}

char peek_prev(struct Lexer *lexer) {
  struct Line *line = lexer->doc->buf[lexer->line];
  if (!lexer->curr) return '\0';
  return line->buf[lexer->curr - 1];
}

void scan_token(struct Lexer *lexer) {
  char ch = advance(lexer);
  switch (ch) {
  case '(':
  case ')':
  case '[':
  case ']':
  case '{':
  case '}':
    add_token(lexer, TOKEN_DELIMITER);
    break;

  case '#':
    scan_directive(lexer);
    break;

  case '\"':
  case '\'':
    scan_quoted_literal(lexer, ch);
    break;

  default:
    if (isdigit(ch)) {
      scan_number_literal(lexer);
    } else if (is_alpha(ch)) {
      scan_identifier(lexer);
    }
  }
}

void scan_quoted_literal(struct Lexer *lexer, char delimiter) {
  while (peek(lexer) != delimiter && !is_at_end(lexer)) {
    if (peek(lexer) == '\\') advance(lexer);
    advance(lexer);
  }
  if (peek(lexer) == '\0') return;
  advance(lexer);
  enum TokenGroup group = delimiter == '\'' ? TOKEN_LITERAL_SYMBOL : TOKEN_LITERAL_STRING;
  add_token(lexer, group);
}

void scan_directive(struct Lexer *lexer) {
  while (peek(lexer) == ' ') {
    advance(lexer);
  }
  while (is_alpha_numeric(peek(lexer))) {
    advance(lexer);
  }
  add_token(lexer, TOKEN_DIRECTIVE);
}

void scan_number_literal(struct Lexer *lexer) {
  while (isdigit(peek(lexer))) {
    advance(lexer);
  }
  if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
    advance(lexer);
    while (isdigit(peek(lexer))) {
      advance(lexer);
    }
  }
  add_token(lexer, TOKEN_LITERAL_NUMBER);
}

void scan_identifier(struct Lexer *lexer) {
  while (is_alpha_numeric(peek(lexer))) {
    advance(lexer);
  }
  struct Line *line = lexer->doc->buf[lexer->line];
  size_t len = lexer->curr - lexer->start;
  char *identifier = (char *)xmalloc(len + 1);
  strncpy(identifier, line->buf + lexer->start, len);
  identifier[len] = '\0';

  enum TokenGroup group = -1;
  size_t keywords_len = sizeof(keywords) / sizeof(keywords[0]);
  size_t data_types_len = sizeof(data_types) / sizeof(data_types[0]);
  for (int i = 0; i < MAX(keywords_len, data_types_len); i++) {
    if (i < keywords_len && !strcmp(identifier, keywords[i])) {
      group = TOKEN_KEYWORD;
      break;
    }
    if (i < data_types_len && !strcmp(identifier, data_types[i])) {
      group = TOKEN_DATA_TYPE;
      break;
    }
  }

  if (group == -1) {
    while (peek(lexer) == ' ') {
      advance(lexer);
    }
    if (peek(lexer) == '(') group = TOKEN_FUNCTION;
  }
  if (group != -1) add_token(lexer, group);
}

void add_token(struct Lexer *lexer, enum TokenGroup group) {
  struct TokenLine *line = lexer->lines[lexer->line];
  line->len++;
  if (line->len >= line->size) {
    line->size = line->len + ADDITIONAL_REALLOCATION;
    line->buf = (struct Token *)xrealloc(line->buf, line->size * sizeof(struct Token));
  }
  struct Token *token = &line->buf[line->len - 1];
  token->group = group;
  token->start = lexer->start;
  token->len = lexer->curr - lexer->start;
}

void free_tokens(struct Document *doc) {
  if (doc->tokens.buf) {
    for (int i = 0; i < doc->tokens.len; i++) {
      free(doc->tokens.buf[i]);
    }
    free(doc->tokens.buf);
  }
  doc->tokens = (struct Tokens){NULL, 0};
}

bool is_alpha(char ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_'; }
bool is_alpha_numeric(char ch) { return is_alpha(ch) || isdigit(ch); }
bool is_at_end(struct Lexer *lexer) { return lexer->curr >= lexer->doc->buf[lexer->doc->len - 1]->len && lexer->line >= lexer->doc->len - 1; }
