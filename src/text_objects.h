#ifndef TEXT_OBJECTS_INCLUDED
#define TEXT_OBJECTS_INCLUDED

#include "types.h"

struct Vec4 get_word_bounds(struct Document *doc, size_t count, bool is_big_word);

struct Vec4 get_motion_object_bounds(struct Document *doc, enum EditorMode mode, struct DinamicMapping mapping);

#endif
