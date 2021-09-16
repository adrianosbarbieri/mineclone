cmake_minimum_required(VERSION 3.8)
project(mineclone)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

configure_file("Icons.png" "${CMAKE_CURRENT_BINARY_DIR}/Icons.png" COPYONLY)

add_executable(${PROJECT_NAME} "main.cpp" "bit_helper.cpp" "main.h" "bit_helper.h")
target_link_libraries(${PROJECT_NAME} "sfml-window" "sfml-system" "sfml-graphics")
