cmake_minimum_required(VERSION 3.10)

project(InterpolationTask VERSION 1.0)

# Устанавливаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Создаем исполняемый файл
add_executable(interpolation main.cpp)

# Линкуем математическую библиотеку (для exp, abs и других функций)
target_link_libraries(interpolation m)