cmake_minimum_required(VERSION 3.10)
project(WordCountParallel C)

# Configuração rigorosa de flags exigida pela especificação
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Wextra")

# Localiza e vincula a biblioteca pthreads nativa de forma portável
find_package(Threads REQUIRED)

# Inclui os fontes do diretório src
file(GLOB SOURCES "src/*.c")

add_executable(word_count_exe ${SOURCES})

# Linka com Threads::Threads de forma explícita
target_link_libraries(word_count_exe PRIVATE Threads::Threads)