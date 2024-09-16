#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "share.h"

void get_wchars_from_file(const char *filename, wchar_t **buffer) {
    FILE *file;
    size_t buffer_size = INITIAL_BUFFER;
    size_t num_chars_read = 0;

    setlocale(LC_ALL, "");

    file = fopen(filename, "r, ccs=UTF-8");
    if (file == NULL) {
        perror("Error abriendo el archivo");
        exit(1);
    }

    *buffer = (wchar_t *)malloc(buffer_size * sizeof(wchar_t));
    if (*buffer == NULL) {
        perror("Error al momento de solicitar la memoria para el buffer");
        fclose(file);
        exit(1);
    }

    wint_t wchar;
    while ((wchar = fgetwc(file)) != WEOF) {
        if (num_chars_read > buffer_size - 1) {
            buffer_size *= 2; 
            wchar_t *new_buffer = (wchar_t *)realloc(*buffer, buffer_size * sizeof(wchar_t));
            if (new_buffer == NULL) {
                perror("Error realojando la memoria");
                free(*buffer);
                fclose(file);
                exit(1);
            }
            *buffer = new_buffer;
        }

        (*buffer)[num_chars_read++] = wchar;
    }

    (*buffer)[num_chars_read] = L'\0';

    fclose(file);
}

long write_directory_metadata(FILE *binary_file, const struct directory_metadata* metadata) {
    
    size_t dirname_length = strlen(metadata->directory);
    fwrite(&dirname_length, sizeof(size_t), 1, binary_file);
    
    fwrite(metadata->directory, sizeof(char), dirname_length, binary_file);
    
    fwrite(&(metadata->num_txt_files), sizeof(int), 1, binary_file);

    long position_before_offsets = ftell(binary_file);

    fwrite(metadata->offsets, sizeof(size_t), metadata->num_txt_files, binary_file);

    return position_before_offsets;
}

void serialize_huffman_tree(struct min_heap_node* root, FILE* file) {
    if (root == NULL) {
        fputc(0, file);
        return;
    }

    fputc(1, file);
    fwrite(&(root->data), sizeof(wchar_t), 1, file);
    fwrite(&(root->freq), sizeof(unsigned), 1, file);

    serialize_huffman_tree(root->left, file);
    serialize_huffman_tree(root->right, file);
}

void write_metadata(size_t offset, const char* filename, size_t size, FILE* file) {
    fwrite(&offset, sizeof(size_t), 1, file);
    size_t filename_length = strlen(filename);
    fwrite(&filename_length, sizeof(size_t), 1, file);
    fwrite(filename, sizeof(char), filename_length, file);

    fwrite(&size, sizeof(size_t), 1, file);
}

const char* extract_filename(const char* filepath) {
    const char* filename = filepath;
    const char* last_slash = strrchr(filepath, '/'); 
    if (last_slash != NULL) {
        filename = last_slash + 1;
    }
    return filename;
}


void write_encoded_bits_to_file(wchar_t *buffer, size_t buffer_size, const char* filepath, struct min_heap_node* huffman_root, struct huffman_code* huffman_codes[], FILE *output_file, size_t* offsets_ptr, int pos) {
    const char* filename = extract_filename(filepath);
    
    size_t offset = ftell(output_file);
    offsets_ptr[pos-1] = offset;

    write_metadata(offset, filename, buffer_size, output_file);
    
    serialize_huffman_tree(huffman_root, output_file);

    unsigned char buffer_byte = 0; 
    
    int bit_count = 0;

    for (size_t i = 0; i < buffer_size; ++i) {

        int* code = huffman_codes[buffer[i]]->code;
        int code_length = huffman_codes[buffer[i]]->length; 

        for (int j = 0; j < code_length; ++j) {

            buffer_byte |= (code[j] << (7 - bit_count));
            ++bit_count;

            if (bit_count == 8) {
                fputc(buffer_byte, output_file);
                buffer_byte = 0;
                bit_count = 0;
            }
        }
    }

    if (bit_count > 0) {
        fputc(buffer_byte, output_file);
    }
}


void read_directory_metadata(struct directory_metadata* metadata, FILE* binary_file) {
    size_t dirname_length;
    if (fread(&dirname_length, sizeof(size_t), 1, binary_file) != 1) {
        perror("Error leyendo la longitud del path");
        exit(EXIT_FAILURE);
    }
    
    char* dirname_buffer = (char*)malloc((dirname_length + 1) * sizeof(char));
    if (dirname_buffer == NULL) {
        perror("Error alojando la memoria para le buffer de los nombres");
        exit(EXIT_FAILURE);
    }

    if (fread(dirname_buffer, sizeof(char), dirname_length, binary_file) != dirname_length) {
        perror("Error leyendo el nombre de los archivos");
        exit(EXIT_FAILURE);
    }
    dirname_buffer[dirname_length] = '\0';

    metadata->directory = dirname_buffer;
    
    if (fread(&(metadata->num_txt_files), sizeof(int), 1, binary_file) != 1) {
        perror("Error leyendo el numero de archivos de texto");
        exit(EXIT_FAILURE);
    }

    size_t numOffsets = metadata->num_txt_files;
    if (fread(metadata->offsets, sizeof(size_t), numOffsets, binary_file) != numOffsets) {
        perror("Error leyendo el arreglo de posiciones del archivo");
        exit(EXIT_FAILURE);
    }
}

const char* create_output_dir(const char* dirname) {
    const char* out_path = "output/";
    const char* dir_path = concat_strings(out_path, dirname);
    struct stat st;
    if (stat(dir_path, &st) == -1) {
        if (mkdir(dir_path, 0777) == -1) { 
            if (errno == EEXIST) {

            } else {
                perror("Error creando el folder");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Ruta '%s' creada satisfactoriamente! .\n", dir_path);
        }
        
    }
    return dir_path;
}


const char* concat_strings(const char* str1, const char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t total_len = len1 + len2 + 1; 

    char* result = (char*)malloc(total_len);
    if (result == NULL) {
        perror("Error alojando la memoria para el nuevo string");
        exit(EXIT_FAILURE);
    }

    strcpy(result, str1);

    strcat(result, str2);

    return result;
}

struct min_heap_node* deserialize_huffman_tree(FILE* file) {
    int marker = fgetc(file);

    if (marker == 0) return NULL;

    struct min_heap_node* root = (struct min_heap_node*)malloc(sizeof(struct min_heap_node));
    if (root == NULL) {
        perror("Error al alojar la memoria para el arbol de huffman");
        exit(EXIT_FAILURE);
    }

    fread(&(root->data), sizeof(wchar_t), 1, file);
    fread(&(root->freq), sizeof(unsigned), 1, file);

    root->left = deserialize_huffman_tree(file);
    root->right = deserialize_huffman_tree(file);

    return root;
}

void read_metadata(size_t* offset, const char* filename, size_t* size, FILE* file) {
    if (fread(offset, sizeof(size_t), 1, file) != 1) {
        perror("Error leyendo el offset");
        exit(EXIT_FAILURE);
    }
    
    size_t filename_length;
    if (fread(&filename_length, sizeof(size_t), 1, file) != 1) {
        perror("Error leyendo el tamanno del archivo");
        exit(EXIT_FAILURE);
    }

    if (filename_length == 0 || filename_length > MAX_NAME_SIZE) {
        fprintf(stderr, "Tamanno de nombre invalido %zu\n", filename_length);
        exit(EXIT_FAILURE);
    }
    
    char* filename_buffer = (char*)malloc((filename_length + 1) * sizeof(char));
    if (filename_buffer == NULL) {
        perror("Error al tratar de asignar memoria al nombre del archivo");
        exit(EXIT_FAILURE);
    }

    if (fread(filename_buffer, sizeof(char), filename_length, file) != filename_length) {
        perror("Error al leer el nombre del archivo");
        exit(EXIT_FAILURE);
    }
    filename_buffer[filename_length] = '\0';

    if (fread(size, sizeof(size_t), 1, file) != 1) {
        perror("Error al leer el largo del archivo");
        exit(EXIT_FAILURE);
    }
    
    strcpy((char *) filename, filename_buffer);
    free(filename_buffer);
}

void decompress_and_write_to_file(FILE *source, const char *output_path, int pos) {
    
    if (source == NULL) {
        perror("Error al abrir el archivo binario");
        exit(EXIT_FAILURE);
    }

    size_t file_size;
    char filename[256];
    size_t offset;
    read_metadata(&offset, filename, &file_size, source);

    struct min_heap_node* huffman_root = deserialize_huffman_tree(source);
    if (huffman_root == NULL) {
        perror("Error al alojar la memoria para el nodo");
        exit(EXIT_FAILURE);
    }
    struct min_heap_node* currentNode = huffman_root;

    setlocale(LC_ALL, "");
    
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/%s", output_path, filename);

    FILE *output_file = fopen(file_path, "w+, ccs=UTF-8");

    if (output_file == NULL) {
        perror("Error al abrir el archivo de salida");
        fclose(source);
        exit(EXIT_FAILURE);
    }

    unsigned char buffer;
    size_t bytes_written = 0;
    while (bytes_written < file_size) {
        buffer = fgetc(source);
        for (int i = 7; i >=0; --i) {
            int bit = (buffer >> i) & 1;
            if (bit == 0) {
                currentNode = currentNode->left;
            } else {
                currentNode = currentNode->right;
            }

            if (!(currentNode->left) && !(currentNode->right)) {
                fputwc(currentNode->data, output_file);
                currentNode = huffman_root; 
                bytes_written++;
                if (bytes_written >= file_size) {
                    break;
                }
            }
        }
    }
    
    free_huffman_tree(huffman_root);
    fclose(output_file);
}


struct encode_args* get_all_paths(const char* books_folder){

    int fileCounter = 0;
    struct encode_args* args = (struct encode_args*) malloc(sizeof(struct encode_args));
        
    DIR* dir;
    struct dirent* entrada;

    dir = opendir(books_folder);
    if (dir == NULL) {
        perror("Error al abrir el directorio");
    }

    while ((entrada = readdir(dir)) != NULL) {
        if (strstr(entrada->d_name, ".txt") != NULL) {
            char bookPath[MAX_NAME_SIZE];
            snprintf(bookPath, sizeof(bookPath), "%s/%s", books_folder, entrada->d_name);
            strncpy(args->books[fileCounter], bookPath, MAX_NAME_SIZE - 1);

            args->books[fileCounter][MAX_NAME_SIZE - 1] = '\0';                        
            fileCounter++;
        }
    }
    args->file_count = fileCounter;
    closedir(dir);
    return args;
}
