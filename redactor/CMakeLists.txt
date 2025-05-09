cmake_minimum_required(VERSION 3.10)
project(vim)

set(SOURCES
    source/commands.cpp
    source/dataset.cpp
    source/highlited.cpp
    source/pointer.cpp
    source/window.cpp
    source/window_sizing.cpp
    source/main.cpp
)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})