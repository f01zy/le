#ifndef LEXER_INCLUDED
#define LEXER_INCLUDED

#include "types.h"

void free_tokens(struct Document *doc);
struct Tokens scan_tokens(struct Document *doc);

#endif
