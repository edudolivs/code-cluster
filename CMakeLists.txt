cmake_minimum_required(VERSION 3.14)
project(code_cluster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Coleta todos os .cpp dentro de src/
file(GLOB SOURCES "src/*.cpp")

add_executable(code_cluster ${SOURCES})
target_compile_options(code_cluster PRIVATE -Wall -Wextra)

# Concorrencia (TP3 - Questao 3): std::thread/std::async exigem linkar pthread
find_package(Threads REQUIRED)
target_link_libraries(code_cluster PRIVATE Threads::Threads)

# ---- Testes com Catch2 (TP2 — Questão 4) ----
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(testes tests/test_tp2.cpp)
target_compile_options(testes PRIVATE -Wall -Wextra)
target_include_directories(testes PRIVATE src)
target_link_libraries(testes PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(testes)
