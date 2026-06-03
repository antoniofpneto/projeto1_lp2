cmake_minimum_required(VERSION 3.10)

project(word_counter C)

set(CMAKE_C_STANDARD 11)

add_compile_options(-O2 -Wall -Wextra)

find_package(Threads REQUIRED)

add_executable(
    word_counter
    src/main.c
    src/sequential.c
    src/parallel.c
    src/timer.c
    src/utils.c
)

target_link_libraries(
    word_counter
    Threads::Threads
)