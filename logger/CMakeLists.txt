cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)
project(logger)
add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)