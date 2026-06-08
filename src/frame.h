#ifndef FRAME_INCLUDED
#define FRAME_INCLUDED

#include "types.h"
#include "ui.h"

struct Cell {
  char ch;
  enum RenderMode mode;
  enum ForegroundColor fg;
  enum BackgroundColor bg;
};

struct Cell **create_frame(struct Vec2 size);

#endif
