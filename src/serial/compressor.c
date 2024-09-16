#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "../dependencies/share/share.c"
#include "../dependencies/huffman/huffmancodes.c"
#include "../dependencies/huffman/huffmantree.c"
#include "../dependencies/minheap/minheap.c"
#include "../dependencies/frequency/frequency.c"

void compress_file(const char *input_filepath, FILE *output_file, size_t *offsets, int position) {
    wchar_t *buffer = NULL;
    int frequency_table[CHAR_SET_SIZE] = {0};
    int frequency_table_size;
    struct min_heap_node *huffman_root;
    struct huffman_code *huffman_code_array[MAX_FREQ_TABLE_SIZE] = {NULL};
    int huffman_bits[MAX_CODE_SIZE];
    size_t buffer_size;

    get_wchars_from_file(input_filepath, &buffer);

    char_frequencies(buffer, frequency_table);
    
    frequency_table_size = calculate_freq_table_size(frequency_table);

    huffman_root = build_huffman_tree(frequency_table, frequency_table_size);
    generate_huffman_codes(huffman_root, huffman_bits, 0, huffman_code_array);

    buffer_size = wcslen(buffer);
    write_encoded_bits_to_file(buffer, buffer_size, input_filepath, huffman_root, huffman_code_array, output_file, offsets, position);

    free(buffer);
    free_huffman_tree(huffman_root);
}

int main() {
    FILE *output_file = fopen(OUTPUT_PATH, "wb");
    if (!output_file) {
        perror("Error abriendo el archivo binario");
        exit(EXIT_FAILURE);
    }

    struct encode_args *paths = get_all_paths(BOOKS_PATH);
    if (!paths) {
        perror("Error obteniendo las rutas de los archivos");
        fclose(output_file);
        exit(EXIT_FAILURE);
    }

    struct directory_metadata dir_metadata = {
        .directory = BOOKS_PATH,
        .num_txt_files = paths->file_count,
        .offsets = {0}
    };

    long metadata_offset_pos = write_directory_metadata(output_file, &dir_metadata);

    size_t offsets[TOTAL_BOOKS] = {0};

    for (int i = 0; i < paths->file_count; i++) {
        compress_file(paths->books[i], output_file, offsets, i + 1);
    }

    fseek(output_file, metadata_offset_pos, SEEK_SET);
    fwrite(offsets, sizeof(size_t), paths->file_count, output_file);

    fclose(output_file);
    free(paths);
    
    return EXIT_SUCCESS;
}