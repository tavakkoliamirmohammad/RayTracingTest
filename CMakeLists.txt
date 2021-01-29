cmake_minimum_required(VERSION 3.17)
project(rayTracing)

set(CMAKE_CXX_STANDARD 11)
add_executable(rayTracing main.cpp shader.cpp shader.h texture.cpp texture.h matrix_transform.cpp matrix_transform.h load_model.cpp load_model.h)
target_link_libraries(rayTracing -lGLU -lglut -lGLEW -lGL)