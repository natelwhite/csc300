cmake_minimum_required(VERSION 3.28)

project(CSC300)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # generate compiler_commands.json for clangd

file(
  GLOB SOURCES
  ${PROJECT_SOURCE_DIR}/src/CSC300.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
