cmake_minimum_required(VERSION 3.13)
project(ray_tracing_practice CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(main ./src/main.cpp)
target_compile_options(main PUBLIC -Wall -Wextra)
