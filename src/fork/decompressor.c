#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../dependencies/share/share.c"
#include "../dependencies/huffman/huffmancodes.c"
#include "../dependencies/huffman/huffmantree.c"
#include "../dependencies/minheap/minheap.c"
#include "../dependencies/frequency/frequency.c"


FILE* open_binary_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("Error abriendo el archivo");
        exit(EXIT_FAILURE);
    }
    return file;
}

void decompress_file(const char *output_path, const struct directory_metadata *dir_metadata, const char *dir_path, int file_index) {
    FILE *binary_source = open_binary_file(output_path);

    size_t current_offset = dir_metadata->offsets[file_index];
    fseek(binary_source, current_offset, SEEK_SET);

    decompress_and_write_to_file(binary_source, dir_path, file_index + 1);

    fclose(binary_source);
}

void fork_and_decompress(const struct directory_metadata *dir_metadata, const char *dir_path) {
    pid_t pid;
    int total_files = dir_metadata->num_txt_files;

    for (int i = 0; i < total_files; ++i) {
        pid = fork();

        if (pid == 0) {  
            decompress_file(OUTPUT_PATH, dir_metadata, dir_path, i);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) {  
            perror("Error en el proceso");
            exit(EXIT_FAILURE);
        }
    }

    while (total_files > 0) {
        wait(NULL);
        total_files--;
    }
}

int main() {
    struct directory_metadata dir_metadata;

    FILE *binary_source = open_binary_file(OUTPUT_PATH);
    read_directory_metadata(&dir_metadata, binary_source);
    fclose(binary_source);

    const char *dir_result = create_output_dir(dir_metadata.directory);
    char *dir_path = strdup(concat_strings(dir_result, "/"));

    fork_and_decompress(&dir_metadata, dir_path);

    printf("Cantidad de archivos descomprimidos: %d\n", dir_metadata.num_txt_files);

    free(dir_path);

    return EXIT_SUCCESS;
}
