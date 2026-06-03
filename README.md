## UFPB — Centro de Informática — LPII — 2026.1
## Trabalho Prático 1 • Prof. Carlos Eduardo C. F. Batista

Nome: Antônio Francelino de Pontes Neto
Matrícula: 20240008403

## Problema Escolhido

P3 - Contagem de palavras em arquivo grande

O objetivo é contar palavras em um arquivo de texto
utilizando uma versão sequencial e uma versão paralela
com pthreads.

O arquivo é carregado completamente em memória antes
da medição.

A versão paralela divide o buffer em fatias contíguas e
cada thread conta apenas as palavras que iniciam dentro
da sua fatia.

## Compilação

### CMake

cmake -B build
cmake --build build

### GCC

gcc -O2 -Wall -Wextra -pthread src/*.c -o word_counter

## Execução

./build/word_counter input/texto.txt 4

## Ambiente de Teste

CPU: Intel Core i5-12450H

Núcleos físicos: 8 

Threads lógicas: 12

SO: Windows 11 25H2

Compilador: GCC 6.3.0

Flags: 