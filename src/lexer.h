#ifndef LEXER_INCLUDED
#define LEXER_INCLUDED

#include "memory.h"
#include "types.h"

void free_tokens(struct Document *doc);
struct Tokens scan_tokens(struct Document *doc);

#endif
