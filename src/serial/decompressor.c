#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "../dependencies/share/share.c"
#include "../dependencies/huffman/huffmancodes.c"
#include "../dependencies/huffman/huffmantree.c"
#include "../dependencies/minheap/minheap.c"
#include "../dependencies/frequency/frequency.c"

int main() {
    
    FILE *binary_source = fopen(OUTPUT_PATH, "rb");
    if (binary_source == NULL) {
        perror("Error al abrir el archivo binario para descompresión");
        exit(EXIT_FAILURE);
    }

    struct directory_metadata dir_metadata;
    read_directory_metadata(&dir_metadata, binary_source);

    printf("Cantidad de archivos descomprimidos: %d\n", dir_metadata.num_txt_files);

    const char* dir_result = create_output_dir(dir_metadata.directory);
    if (!dir_result) {
        perror("Error al crear el directorio de salida");
        fclose(binary_source);
        exit(EXIT_FAILURE);
    }

    const char* dir_path = concat_strings(dir_result, "/");
    if (!dir_path) {
        perror("Error al concatenar la ruta del directorio");
        fclose(binary_source);
        free((void *)dir_result);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < dir_metadata.num_txt_files; i++) {
        decompress_and_write_to_file(binary_source, dir_path, i + 1);
    }

    fclose(binary_source);
    free((void *)dir_path);
    free((void *)dir_result);

    return EXIT_SUCCESS;
}
