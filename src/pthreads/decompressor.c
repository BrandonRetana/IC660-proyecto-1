#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <semaphore.h>
#include <pthread.h>

#include "../dependencies/share/share.c"
#include "../dependencies/huffman/huffmancodes.c"
#include "../dependencies/huffman/huffmantree.c"
#include "../dependencies/minheap/minheap.c"
#include "../dependencies/frequency/frequency.c"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* decode_book(void* arg) {
    struct thread_args_d* args = (struct thread_args_d*)arg;

    pthread_mutex_lock(&mutex);
    fseek(args->binary_source, args->offset, SEEK_SET);
    decompress_and_write_to_file(args->binary_source, args->outputPath, args->pos);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    const char* input_file = "output/bin/compressed.bin";
    
    FILE *binary_source = fopen(input_file, "rb");
    if (binary_source == NULL) {
        perror("Error al abrir el binario para la descompresion");
        exit(EXIT_FAILURE);
    }

    struct directory_metadata dir_metadata;
    read_directory_metadata(&dir_metadata, binary_source);

    printf("Cantidad de archivos descomprimidos: %d\n", dir_metadata.num_txt_files);

    const char* dir_result = create_output_dir(dir_metadata.directory);
    char *dir_path = strdup(concat_strings(dir_result, "/"));
    if (dir_path == NULL) {
        perror("Error al crear el directori de salida");
        fclose(binary_source);
        exit(EXIT_FAILURE);
    }

    pthread_t* threads = malloc(dir_metadata.num_txt_files * sizeof(pthread_t));
    struct thread_args_d* thread_args_d = malloc(dir_metadata.num_txt_files * sizeof(struct thread_args_d));

    if (threads == NULL || thread_args_d == NULL) {
        perror("Error al al asignar la memoria");
        fclose(binary_source);
        free(dir_path);
        free(threads);
        free(thread_args_d);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < dir_metadata.num_txt_files; ++i) {
        thread_args_d[i].binary_source = binary_source;
        thread_args_d[i].outputPath = dir_path;
        thread_args_d[i].pos = i + 1;
        thread_args_d[i].offset = dir_metadata.offsets[i];

        if (pthread_create(&threads[i], NULL, decode_book, (void*)&thread_args_d[i]) != 0) {
            perror("Error creando el hilo");
            free(threads);
            free(thread_args_d);
            fclose(binary_source);
            free(dir_path);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < dir_metadata.num_txt_files; ++i) {
        pthread_join(threads[i], NULL);
    }

    fclose(binary_source);
    free(dir_path);
    free(threads);
    free(thread_args_d);
    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}
