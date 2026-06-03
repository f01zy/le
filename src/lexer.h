#ifndef LEXER_INCLUDED
#define LEXER_INCLUDED

#include "memory.h"
#include "types.h"

bool is_alpha(char ch);
bool is_alpha_numeric(char ch);
bool is_at_end(struct Lexer *lexer);
void free_tokens(struct Document *doc);
void add_token(struct Lexer *lexer, enum TokenGroup type);
char advance(struct Lexer *lexer);
char peek(struct Lexer *lexer);
char peek_next(struct Lexer *lexer);
char peek_prev(struct Lexer *lexer);
char peek_next_exept_space(struct Lexer *lexer);
void scan_quoted_literal(struct Lexer *lexer, char delimiter);
void scan_number_literal(struct Lexer *lexer);
void scan_identifier(struct Lexer *lexer);
void scan_directive(struct Lexer *lexer);
void scan_token(struct Lexer *lexer);
struct Tokens scan_tokens(struct Document *doc);

#endif
