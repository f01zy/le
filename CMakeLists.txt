cmake_minimum_required(VERSION 3.5)
project(le 
  VERSION 0.0.1 
  DESCRIPTION "Light terminal vim-like text editor" 
  HOMEPAGE_URL "https://github.com/f01zy/le"
  LANGUAGES C
)

set(SOURCE
  ./src/main.c
  ./src/service.c
  ./src/render.c
  ./src/terminal.c
  ./src/buffer.c
  ./src/memory.c
  ./src/filesystem.c
  ./src/ui.c
  ./src/handlers.c
  ./src/editor_commands.c
  ./src/mappings.c
  ./src/lexer.c
  ./src/path.c
  ./src/selected.c
  ./src/text_objects.c
  ./src/frame.c
  ./src/file_tree.c
)

add_executable(le ${SOURCE})
