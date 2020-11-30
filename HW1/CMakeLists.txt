cmake_minimum_required(VERSION 3.17)

project (kmeans)

set(CMAKE_C_STANDARD 90)
add_compile_options(-Wall -Wextra -pedantic -Werror -ansi -lm)
add_executable (kmeans kmeans.c)
