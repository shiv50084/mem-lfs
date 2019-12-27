cmake_minimum_required(VERSION 3.15)
project(mem_lfs C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(deps/)
add_library(utils OBJECT print_utils.c memory_utils.c)
add_executable(mem_lfs main.c)
target_link_libraries(mem_lfs lfs utils)