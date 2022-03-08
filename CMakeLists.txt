cmake_minimum_required(VERSION 3.10)
project(learnopengl)

set(CMAKE_CXX_STANDARD 20)

add_library(glad libs/glad/include/glad/glad.h
                 libs/glad/include/KHR/khrplatform.h
                 libs/glad/src/glad.c)
include_directories(libs/glad/include)

find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(learnopengl
        src/main.cpp
        src/shader.cpp
        src/shader.h
        src/utils.h
        src/camera.cpp
        src/camera.h
        src/main.h
        src/utils.cpp
        src/common.cpp
        src/common.h
        )

target_link_libraries(learnopengl PRIVATE glfw)
target_link_libraries(learnopengl PRIVATE glad)
target_link_libraries(learnopengl PRIVATE assimp)