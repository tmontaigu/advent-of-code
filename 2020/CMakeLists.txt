cmake_minimum_required(VERSION 3.17)

project(AoC2020 C)

set(CMAKE_C_STANDARD 99)


add_library(aoc STATIC aoc_lib/aoc.c aoc_lib/aoc.h)
target_include_directories(aoc PUBLIC aoc_lib)

add_executable(aoc_1 aoc_1/main.c)
target_link_libraries(aoc_1 PRIVATE aoc)

add_executable(aoc_3 aoc_3/main.c)
target_link_libraries(aoc_3 PRIVATE aoc)

add_executable(aoc_4 aoc_4/main.c)

add_executable(aoc_5 aoc_5/main.c)

add_executable(aoc_6 aoc_6/main.c)

add_executable(aoc_7 aoc_7/main.c)
target_link_libraries(aoc_7 PRIVATE aoc)

add_executable(aoc_8 aoc_8/main.c)
target_link_libraries(aoc_8 PRIVATE aoc)
