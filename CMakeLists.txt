cmake_minimum_required(VERSION 3.28)

project(ProjectTwo)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # generate compiler_commands.json for clangd

file(
  GLOB SOURCES
  ${PROJECT_SOURCE_DIR}/src/ProjectTwo.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
