cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
project(det)
add_executable(${CMAKE_PROJECT_NAME} main.cpp determine.cpp determine_2.cpp test.cpp)