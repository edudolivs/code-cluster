cmake_minimum_required(VERSION 3.10)
project(llm_service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Coleta todos os .cpp dentro de src/
file(GLOB SOURCES "src/*.cpp")

add_executable(llm_service ${SOURCES})
