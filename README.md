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

## Geração da Entrada via Python (P3 - Texto)
Seguindo as diretrizes de modelagem do enunciado, a entrada de dados foi gerada utilizando o script auxiliar `gerar_texto.py`. 

O script simula o comportamento do *Random String Generator* (`random.org/strings`), criando um lote base de 5.000 strings aleatórias (com tamanhos variando dinamicamente entre 3 e 12 caracteres). Utilizando uma estratégia de semente fixa (`random.seed(42)`), esse lote foi replicado e concatenado com separação por espaços até atingir o tamanho alvo de **55 MB** em disco, gerando o arquivo `texto_grande.txt`.

---

## Instruções de Compilação e Execução (Questão 1 - C)

O programa aceita dinamicamente o número de threads desejado como um argumento de linha de comando. Se nenhum valor for passado, o padrão de execução será de 4 threads.

### Compilação e Execução via CMake (Recomendado)
```bash```
# Configura o build em modo de otimização (Release)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compila o projeto
cmake --build build

# Executa variando o número de threads (Exemplo com 4 threads)
./build/word_count_exe 4

# Compilação direta com flags estritas e biblioteca de threads
gcc -O2 -Wall -Wextra -pthread src/main.c -o meu_exe

# Executa variando o número de threads (Exemplo com 8 threads)
./meu_exe 8

## Ambiente de Teste das Medições

Processador (CPU): AMD Ryzen 5 5600H

Arquitetura: x86_64 (6 Núcleos Físicos / 12 Threads Lógicas)

Sistema Operacional: Linux Ubuntu 22.04 LTS

Compilador: GCC versão 11.4.0

Flags de Otimização Utilizadas: -O2 -Wall -Wextra -pthread

## Metodologia de Cronometragem (Questão 2 - D)

Isolamento da Região de Computação: A leitura do arquivo texto_grande.txt do disco para a memória RAM e a alocação dos buffers principais do sistema foram executadas inteiramente na etapa de Setup, ficando 
completamente fora da janela de medição de tempo.

Relógio Monotônico Nativo: Utilizou-se a função clock_gettime(CLOCK_MONOTONIC, &t0) para capturar os tempos de execução com precisão de nanossegundos.

Loop Estatístico e Descarte de Aquecimento: Foram realizadas 25 execuções consecutivas para cada configuração de threads. A primeira rodada de cada teste foi integralmente descartada como aquecimento (warmup) para evitar distorções causadas por caches frios de memória e carregamento inicial de páginas de registradores. O tempo final reportado Tseq e Tpar consiste na média aritmética das 24 rodadas subsequentes estabilizadas.

## Tabela de Desempenho e Escalabilidade (Questão 3 - D e Questão 4 - A)

| Threads | Tempo Médio (s) | Speedup Real | Eficiência Global |
|----------|------------------|--------------|-------------------|
| 1 (seq)  | 0.0524 s         | 1.00x        | 100.0%            |
| 2        | 0.0347 s         | 1.51x        | 75.5%             |
| 4        | 0.0298 s         | 1.75x        | 43.7%             |
| 8        | 0.0294 s         | 1.78x        | 22.2%             |

## Gráfico de Escalabilidade (Questão 4 - B)

Speedup Real
  2.0x |                                     o (1.78x)
       |                        o (1.75x)
  1.5x |           o (1.51x)
       |
  1.0x | o (1.00x)
       +--------------------------------------------
         1         2            4            8    
                                       Nº de Threads

## Discussão dos Resultados e Análise de Escalabilidade (Questão 4 - C)

Como observado nos dados experimentais, o aumento no número de threads não se traduziu em um ganho de desempenho puramente linear, atingindo um platô de saturação de 1.78x de speedup máximo. Esse comportamento sublinear rígido é perfeitamente justificável por dois fatores arquiteturais principais:

Limite de Banda de Memória (Memory Bandwidth Wall / Carga Memory-Bound): O algoritmo de contagem de palavras realiza um trabalho computacional extremamente leve por byte carregado (apenas uma checagem condicional de caractere via isspace). Consequentemente, quando múltiplas threads em núcleos paralelos tentam varrer simultaneamente fatias contíguas de um buffer linear de 55 MB, elas saturam rapidamente os barramentos de comunicação com a memória RAM. O fator limitante deixa de ser a capacidade de processamento da CPU e passa a ser a velocidade de transporte dos dados. Adicionar mais threads (como ir de 4 para 8) faz com que os núcleos fiquem ociosos esperando pelas requisições de leitura (cache misses lineares), estagnando o tempo.

Overhead de Criação e Junção de Threads: Dado que o tempo total de processamento sequencial da carga de dados é muito baixo (da ordem de centésimos de segundo), o custo temporal que o Sistema Operacional gasta para despachar chamadas de sistema pthread_create, alocar pilhas de execução e sincronizar os retornos em pthread_join passa a representar uma fração significativa do tempo de execução. Esse custo administrativo mitiga os benefícios obtidos com a divisão paralela do processamento do texto, gerando uma queda acentuada na métrica de eficiência global à medida que a quantidade de threads cresce.