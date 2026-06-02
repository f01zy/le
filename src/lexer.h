#ifndef LEXER_INCLUDED
#define LEXER_INCLUDED

#include "memory.h"
#include "types.h"

void free_tokens(struct Document *doc);
void token_string(struct Lexer *lexer);
void add_token(struct Lexer *lexer, enum TokenType type);
char advance(struct Lexer *lexer);
char peek(struct Lexer *lexer);
void scan_token(struct Lexer *lexer);
bool is_at_end(struct Lexer *lexer);
struct Tokens scan_tokens(struct Document *doc);

#endif
