#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <pthread.h>

#include "../dependencies/share/share.c"
#include "../dependencies/huffman/huffmancodes.c"
#include "../dependencies/huffman/huffmantree.c"
#include "../dependencies/minheap/minheap.c"
#include "../dependencies/frequency/frequency.c"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void encode(char *input_file, FILE *binary_output, int pos, size_t* offsets_ptr) {
    wchar_t *buffer = NULL;
    get_wchars_from_file(input_file, &buffer);

    int freq_table[CHAR_SET_SIZE] = {0};
    char_frequencies(buffer, freq_table);

    int freq_table_size = calculate_freq_table_size(freq_table);

    struct min_heap_node* huffman_root = build_huffman_tree(freq_table, freq_table_size);

    struct huffman_code* huffman_codesArray[MAX_FREQ_TABLE_SIZE] = {NULL};
    int bits[MAX_CODE_SIZE];

    generate_huffman_codes(huffman_root, bits, 0, huffman_codesArray);

    size_t buffer_size = wcslen(buffer);

    pthread_mutex_lock(&mutex);
    write_encoded_bits_to_file(buffer, buffer_size, input_file, huffman_root, huffman_codesArray, binary_output, offsets_ptr, pos);
    pthread_mutex_unlock(&mutex);

    free(buffer);
    free_huffman_tree(huffman_root);
}

void* encode_book(void* arg) {
    struct thread_args_c* args = (struct thread_args_c*)arg;

    encode(args->bookFile, args->outputFile, args->pos, args->offsets_ptr);

    return NULL;
}

int main() {

    FILE *binary_output = fopen(OUTPUT_PATH, "wb");
    if (binary_output == NULL) {
        perror("Error al abrir el binario");
        exit(EXIT_FAILURE);
    }

    struct encode_args *paths = get_all_paths(BOOKS_PATH);
    if (paths == NULL) {
        perror("Error al obtener las rutas de los libros a procesar");
        fclose(binary_output);
        exit(EXIT_FAILURE);
    }

    struct directory_metadata dir_metadata = {
        .directory = BOOKS_PATH,
        .num_txt_files = paths->file_count,
        .offsets = {0} 
    };

    long offsets_pos = write_directory_metadata(binary_output, &dir_metadata);

    pthread_t* threads = malloc(paths->file_count * sizeof(pthread_t));
    struct thread_args_c* thread_args_c = malloc(paths->file_count * sizeof(struct thread_args_c));
    size_t* offsets = calloc(paths->file_count, sizeof(size_t));

    if (threads == NULL || thread_args_c == NULL || offsets == NULL) {
        perror("Error al asignar memoria a los hilos");
        fclose(binary_output);
        free(paths);
        free(threads);
        free(thread_args_c);
        free(offsets);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < paths->file_count; ++i) {

        thread_args_c[i].bookFile = paths->books[i];
        thread_args_c[i].outputFile = binary_output;
        thread_args_c[i].pos = i + 1;
        thread_args_c[i].offsets_ptr = offsets;

        if (pthread_create(&threads[i], NULL, encode_book, (void*)&thread_args_c[i]) != 0) {
            perror("Error creando el hilo");
            free(paths);
            free(threads);
            free(thread_args_c);
            free(offsets);
            fclose(binary_output);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < paths->file_count; ++i) {
        pthread_join(threads[i], NULL);
    }

    fseek(binary_output, offsets_pos, SEEK_SET);
    fwrite(offsets, sizeof(size_t), paths->file_count, binary_output);

    fclose(binary_output);
    free(paths);
    free(threads);
    free(thread_args_c);
    free(offsets);
    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}