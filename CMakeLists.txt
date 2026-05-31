cmake_minimum_required(VERSION 3.5)
project(editor)

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
)

add_executable(editor ${SOURCE})
