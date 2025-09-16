cmake_minimum_required(VERSION 3.10)
project(NumericalMethods)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(solver main.cpp numeric_methods.cpp)

add_executable(sphere sphere.cpp numeric_methods.cpp)

target_link_libraries(solver m)
target_link_libraries(sphere m)

add_custom_target(run-solver
    COMMAND ./solver
    DEPENDS solver
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(run-sphere
    COMMAND ./sphere
    DEPENDS sphere
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)