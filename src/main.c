#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>

#define NUM_RUNS 25  // Conforme Questão 3(C)

typedef struct {
    const char *text_buffer;
    long start_index;
    long slice_size;
    long total_size;
    long partial_count; 
} ThreadArg;

// --- BASELINE SEQUENCIAL ORIGINAL (Questão 2) ---
long count_words_sequential(const char *texto, long tam) {
    long total = 0;
    int dentro = 0;
    for (long i = 0; i < tam; i++) {
        if (isspace((unsigned char) texto[i])) {
            dentro = 0;
        } else if (!dentro) {
            dentro = 1;
            total++;
        }
    }
    return total;
}

// --- WORKER PARALELO COM TRATAMENTO DE FRONTEIRA (Questão 3) ---
void *word_counter_worker(void *arg) {
    ThreadArg *t_data = (ThreadArg *)arg;
    long start = t_data->start_index;
    long size = t_data->slice_size;
    const char *text = t_data->text_buffer;
    
    long local_count = 0;
    int dentro = 0;

    // Regra da Fronteira Estrita (Questão 3 - P3)
    if (start > 0) {
        if (!isspace((unsigned char)text[start]) && !isspace((unsigned char)text[start - 1])) {
            dentro = 1; // A thread anterior conta essa palavra, então pulamos o fragmento inicial
        }
    }

    for (long i = 0; i < size; i++) {
        long current_idx = start + i;
        if (current_idx >= t_data->total_size) break;

        if (isspace((unsigned char)text[current_idx])) {
            dentro = 0;
        } else if (!dentro) {
            dentro = 1;
            local_count++;
        }
    }

    t_data->partial_count = local_count;
    pthread_exit(NULL);
}

// --- CONTROLADOR DA CRONOMETRAGEM PARALELA ---
long run_parallel_calculation(const char *text_buffer, long total_size, int num_threads) {
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    ThreadArg *args = malloc(num_threads * sizeof(ThreadArg));
    
    long base_slice = total_size / num_threads;
    long remainder = total_size % num_threads;
    long current_offset = 0;

    for (int i = 0; i < num_threads; i++) {
        args[i].text_buffer = text_buffer;
        args[i].start_index = current_offset;
        args[i].slice_size = base_slice + (i < remainder ? 1 : 0);
        args[i].total_size = total_size;
        args[i].partial_count = 0;
        current_offset += args[i].slice_size;
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, word_counter_worker, &args[i]);
    }

    long aggregate_words = 0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        aggregate_words += args[i].partial_count;
    }

    free(threads);
    free(args);
    return aggregate_words;
}

int main(int argc, char *argv[]) {
    int target_threads = 4; 
    if (argc >= 2) {
        target_threads = atoi(argv[1]);
        if (target_threads < 1) target_threads = 1;
    }

    printf("====================================================\n");
    printf("   LPII - TRABALHO PRATICO 1 - LEITURA DE ARQUIVO   \n");
    printf("====================================================\n\n");

    // --- SETUP: LEITURA DO ARQUIVO FORA DO CRONÔMETRO (Questão 2(A) e 3(A)) ---
    printf("[SETUP] Lendo 'texto_grande.txt' para a memória RAM...\n");
    FILE *file = fopen("texto_grande.txt", "rb");
    if (!file) {
        perror("Erro ao abrir o arquivo texto_grande.txt. Execute o script Python primeiro");
        return EXIT_FAILURE;
    }

    // Descobre o tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long data_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *text_data = malloc(data_size + 1);
    if (!text_data) {
        perror("Erro ao alocar memória para o texto");
        fclose(file);
        return EXIT_FAILURE;
    }

    long bytes_read = fread(text_data, 1, data_size, file);
    text_data[bytes_read] = '\0';
    fclose(file);
    printf("[SETUP] Sucesso! %ld bytes carregados na RAM.\n\n", bytes_read);

    struct timespec t0, t1;
    double accumulated_time = 0.0;
    long seq_result = 0;

    // --- BASELINE SEQUENCIAL ---
    printf("[EXEC] Executando Baseline Sequencial (%d rodadas)... \n", NUM_RUNS);
    for (int r = 0; r < NUM_RUNS; r++) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        seq_result = count_words_sequential(text_data, bytes_read);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        
        if (r > 0) { // Descarte de aquecimento (Questão 2(D))
            accumulated_time += (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
        }
    }
    double t_seq = accumulated_time / (NUM_RUNS - 1);
    printf(">> T_seq Médio: %7.4f segundos.\n", t_seq);
    printf(">> Palavras contadas (Sequencial): %ld\n\n", seq_result);

    // --- VERSÃO PARALELA ---
    printf("[EXEC] Executando Concorrente com %d Threads (%d rodadas)... \n", target_threads, NUM_RUNS);
    accumulated_time = 0.0;
    long par_result = 0;

    for (int r = 0; r < NUM_RUNS; r++) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        par_result = run_parallel_calculation(text_data, bytes_read, target_threads);
        clock_gettime(CLOCK_MONOTONIC, &t1);

        if (r > 0) { // Descarte de aquecimento (Questão 3(C))
            accumulated_time += (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
        }
    }
    double t_par = accumulated_time / (NUM_RUNS - 1);
    printf(">> T_par Médio: %7.4f segundos.\n", t_par);
    printf(">> Palavras contadas (Paralela):   %ld\n", par_result);

    // --- ANCORA DE CORRETUDE ---
    printf("\n[VALIDACAO] ");
    if (seq_result == par_result) {
        printf("Resultado: OK (As contagens batem exatamente!)\n");
    } else {
        printf("Resultado: FALHA (Erro na regra de fronteira!)\n");
    }

    double speedup = t_seq / t_par;
    printf("\n----------------------------------------------------\n");
    printf(" Speedup Alcançado com %d Threads: %4.2fx\n", target_threads, speedup);
    printf("----------------------------------------------------\n");

    free(text_data);
    return 0;
}