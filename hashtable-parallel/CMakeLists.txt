cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)
project(hashtable-parallel)

set(SOURCES
    source/main.cpp
    source/hashtable.cpp
    test/test.cpp
)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})