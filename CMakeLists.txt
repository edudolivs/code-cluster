cmake_minimum_required(VERSION 3.14)
project(code_cluster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Coleta todos os .cpp dentro de src/, exceto gui_main.cpp (pertence ao
# alvo separado 'gui' da Questao 6 — ver bloco Qt6 no final deste arquivo)
file(GLOB SOURCES "src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "gui_main\\.cpp$")

add_executable(code_cluster ${SOURCES})
target_compile_options(code_cluster PRIVATE -Wall -Wextra)

# Concorrencia (TP3 - Questao 3): std::thread/std::async exigem linkar pthread
find_package(Threads REQUIRED)
target_link_libraries(code_cluster PRIVATE Threads::Threads)

include(FetchContent)

# ---- Serializacao JSON (TP3 - Questao 4) ----
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)
target_link_libraries(code_cluster PRIVATE nlohmann_json::nlohmann_json)

# ---- Testes com Catch2 (TP2 — Questão 4) ----
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

# ---- Testes do TP3 (Questao 5, extra) — alvo separado do TP2 ----
add_executable(testes_tp3 tests/test_tp3.cpp)
target_compile_options(testes_tp3 PRIVATE -Wall -Wextra)
target_include_directories(testes_tp3 PRIVATE src)
target_link_libraries(testes_tp3 PRIVATE Catch2::Catch2WithMain nlohmann_json::nlohmann_json)

include(CTest)
include(Catch)
catch_discover_tests(testes)
catch_discover_tests(testes_tp3)

# ---- Interface grafica com Qt (TP3 - Questao 6, extra) ----
# Guardado por Qt6_FOUND: o build de code_cluster/testes/testes_tp3 nunca
# quebra em uma maquina sem Qt6 instalado (o alvo 'gui' so existe se achar).
find_package(Qt6 QUIET COMPONENTS Widgets)
if(Qt6_FOUND)
    set(CMAKE_AUTOMOC ON)
    qt_standard_project_setup()
    qt_add_executable(gui src/gui_main.cpp src/main_window.hpp)
    target_include_directories(gui PRIVATE src)
    target_link_libraries(gui PRIVATE Qt6::Widgets nlohmann_json::nlohmann_json)
    target_compile_options(gui PRIVATE -Wall -Wextra)
else()
    message(STATUS "Qt6 nao encontrado — alvo 'gui' nao sera criado (brew install qt)")
endif()
