cmake_minimum_required(VERSION 3.17)
project(rayTracing)

set(CMAKE_CXX_STANDARD 11)
add_executable(rayTracing main.cpp objLoader.cpp objLoader.h shader.cpp shader.h texture.cpp texture.h matrix_transform.cpp matrix_transform.h)
target_link_libraries(rayTracing -lGLU -lglut -lGLEW -lGL)