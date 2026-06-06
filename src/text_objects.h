#ifndef TEXT_OBJECTS_INCLUDED
#define TEXT_OBJECTS_INCLUDED

#include "types.h"

enum WordGroup {
  WORD_GROUP_SPACE,
  WORD_GROUP_SPECIAL,
  WORD_GROUP_ALPHA,
};

struct Vec4 get_motion_object_bounds(struct Document *doc, enum EditorMode mode, struct DinamicMapping mapping);

#endif
