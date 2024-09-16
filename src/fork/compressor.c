#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <dirent.h>

#include "../dependencies/share/share.c"
#include "../dependencies/huffman/huffmancodes.c"
#include "../dependencies/huffman/huffmantree.c"
#include "../dependencies/minheap/minheap.c"
#include "../dependencies/frequency/frequency.c"



void initialize_shared_memory(int *shmid_ptr, key_t key, size_t **shared_offsets) {
    *shmid_ptr = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (*shmid_ptr < 0) {
        perror("error en la memoria compartida");
        exit(EXIT_FAILURE);
    }

    *shared_offsets = shmat(*shmid_ptr, NULL, 0);
    if (*shared_offsets == (size_t *) -1) {
        perror("error en la memoria compartida");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < TOTAL_BOOKS; ++i) {
        (*shared_offsets)[i] = 0;
    }
}

void perform_encoding(const char *input_filepath, FILE *temp_output_file, size_t *offsets, int position) {
    wchar_t *buffer;
    int frequency_table[CHAR_SET_SIZE] = {0};
    int frequency_table_size;
    struct min_heap_node* huffman_root;
    struct huffman_code* huffman_code_array[MAX_FREQ_TABLE_SIZE] = {NULL};
    int bits[MAX_CODE_SIZE];
    size_t buffer_size;

    get_wchars_from_file(input_filepath, &buffer);
    char_frequencies(buffer, frequency_table);
    frequency_table_size = calculate_freq_table_size(frequency_table);
    huffman_root = build_huffman_tree(frequency_table, frequency_table_size);
    generate_huffman_codes(huffman_root, bits, 0, huffman_code_array);

    buffer_size = wcslen(buffer);
    write_encoded_bits_to_file(buffer, buffer_size, input_filepath, huffman_root, huffman_code_array, temp_output_file, offsets, position);

    free(buffer);
}

void retrieve_temp_filepaths(char temp_filenames[TOTAL_BOOKS][TEMP_FILENAME_LEN]) {
    DIR* temp_dir = opendir(TEMP_PATH);
    if (!temp_dir) {
        perror("error en el directorio temporal");
        exit(EXIT_FAILURE);
    }

    int file_count = 0;
    struct dirent* dir_entry;
    while ((dir_entry = readdir(temp_dir)) != NULL) {
        if (strstr(dir_entry->d_name, ".bin")) {
            snprintf(temp_filenames[file_count], sizeof(temp_filenames[file_count]), "output/temp/%s", dir_entry->d_name);
            file_count++;
        }
    }
    closedir(temp_dir);
}

int main() {
    int shmid;
    size_t *shared_offsets;
    key_t key = IPC_PRIVATE;

    struct encode_args *encode_paths;

    FILE *output_file;
    long metadata_offset_position;
    pid_t child_pid;

    initialize_shared_memory(&shmid, key, &shared_offsets);

    output_file = fopen(OUTPUT_PATH, "wb");
    if (!output_file) {
        perror("error abriendo el archivo");
        exit(EXIT_FAILURE);
    }

    char temp_filenames[TOTAL_BOOKS][TEMP_FILENAME_LEN];
    encode_paths = get_all_paths(BOOKS_PATH);

    struct directory_metadata directory_metadata = {
        .directory = BOOKS_PATH,
        .num_txt_files = encode_paths->file_count,
        .offsets = {0}
    };

    metadata_offset_position = write_directory_metadata(output_file, &directory_metadata);

    for (int i = 0; i < TOTAL_BOOKS; ++i) {
        child_pid = fork();
        if (child_pid == 0) {
            FILE *temp_file;
            size_t temp_offsets[TOTAL_BOOKS] = {0};
            char temp_filepath[TEMP_FILENAME_LEN];

            snprintf(temp_filepath, sizeof(temp_filepath), "output/temp/temp_%d.bin", getpid());

            temp_file = fopen(temp_filepath, "wb");
            if (!temp_file) {
                perror("error abriendo el archivo temporal");
                exit(EXIT_FAILURE);
            }

            perform_encoding(encode_paths->books[i], temp_file, temp_offsets, i);

            fclose(temp_file);
            exit(EXIT_SUCCESS);
        } else if (child_pid < 0) {
            perror("error en el proceso");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < TOTAL_BOOKS; ++i) {
        wait(NULL);
    }

    retrieve_temp_filepaths(temp_filenames);

    for (int i = 0; i < TOTAL_BOOKS; ++i) {
        FILE *temp_file = fopen(temp_filenames[i], "rb");
        if (!temp_file) {
            perror("error al tratar de leer el archivo temporal");
            exit(EXIT_FAILURE);
        }

        fseek(temp_file, 0, SEEK_SET);
        unsigned char buffer[BUFFER_SIZE];
        size_t bytes_read;

        shared_offsets[i] = ftell(output_file);
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), temp_file)) > 0) {
            fwrite(buffer, 1, bytes_read, output_file);
        }

        fclose(temp_file);
    }

    fseek(output_file, metadata_offset_position, SEEK_SET);
    fwrite(shared_offsets, sizeof(size_t), encode_paths->file_count, output_file);

    shmdt(shared_offsets);
    shmctl(shmid, IPC_RMID, NULL);
    fclose(output_file);
    free(encode_paths);

    return EXIT_SUCCESS;
}
