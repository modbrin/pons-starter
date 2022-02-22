cmake_minimum_required(VERSION 3.10)
project(learnopengl)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(gli CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(learnopengl
        src/main.cpp
        src/shader.cpp
        src/shader.h
        src/utils.h
        src/camera.cpp
        src/camera.h
        src/main.h
        src/utils.cpp
        )

target_link_libraries(learnopengl PRIVATE glfw)
target_link_libraries(learnopengl PRIVATE glad::glad)
target_link_libraries(learnopengl PRIVATE gli)
target_link_libraries(learnopengl PRIVATE glm::glm)