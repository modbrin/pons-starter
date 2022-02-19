cmake_minimum_required(VERSION 3.21)
project(learnopengl)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_executable(learnopengl main.cpp shader.cpp shader.h)

target_link_libraries(learnopengl PRIVATE glfw)
target_link_libraries(learnopengl PRIVATE glad::glad)
