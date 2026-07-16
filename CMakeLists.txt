cmake_minimum_required(VERSION 3.10)
project(code_cluster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Coleta todos os .cpp dentro de src/
file(GLOB SOURCES "src/*.cpp")

add_executable(code_cluster ${SOURCES})

target_compile_options(code_cluster PRIVATE -Wall -Wextra)
