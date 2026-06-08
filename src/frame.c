#include "frame.h"
#include "memory.h"

struct Cell **create_frame(struct Vec2 size) {
  struct Cell **frame = (struct Cell **)xmalloc(size.y * sizeof(struct Cell *));
  for (int i = 0; i < size.y; i++) {
    frame[i] = (struct Cell *)xcalloc(size.x, sizeof(struct Cell));
  }
  return frame;
}
