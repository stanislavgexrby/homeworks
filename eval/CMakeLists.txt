cmake_minimum_required(VERSION 3.20)
project(NumericalMethods)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем флаги компиляции для отладки и предупреждений
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# Исполняемые файлы
add_executable(test_solver test_main.cpp numeric_methods.cpp)
add_executable(sphere_solver sphere_main.cpp numeric_methods.cpp)

# Линковка с математической библиотекой (для функций sin, cos, sqrt и т.д.)
target_link_libraries(test_solver m)
target_link_libraries(sphere_solver m)

# Опциональные цели для удобства разработки
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
)

# Цель для запуска тестового решателя
add_custom_target(run-test
    COMMAND ./test_solver
    DEPENDS test_solver
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Цель для запуска решателя задачи про шар
add_custom_target(run-sphere
    COMMAND ./sphere_solver
    DEPENDS sphere_solver
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)