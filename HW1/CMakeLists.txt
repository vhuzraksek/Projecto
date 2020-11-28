cmake_minimum_required (VERSION 2.8.11)

project (kmeans)

set(CMAKE_C_STANDARD 90)
add_executable (kmeans kmeans.c)
add_compile_options(-Wall -Wextra -pedantic -Werror -ansi -lm)
