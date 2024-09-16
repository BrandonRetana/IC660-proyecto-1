#ifndef SHARE_H
#define SHARE_H

#include "../global/vars.h"
#include "../global/structs.h"
#include "../huffman/huffman.h"

struct encode_args* get_all_paths(const char* books_folder);

void get_wchars_from_file(const char *filename, wchar_t **buffer);

void write_encoded_bits_to_file(wchar_t *buffer, size_t buffer_size, const char* filename, struct min_heap_node* huffman_root, struct huffman_code* huffman_codes[], FILE *output_file, size_t* offsets_ptr, int pos);

struct min_heap_node* deserialize_huffman_tree(FILE* file);

void read_metadata(size_t* offset, const char* filename, size_t* size, FILE* file);

void decompress_and_write_to_file(FILE *source, const char *output_path, int pos);

long write_directory_metadata(FILE *binary_file, const struct directory_metadata* metadata);

void read_directory_metadata(struct directory_metadata* metadata, FILE* binary_file);

const char* create_output_dir(const char* dirname);

const char* concat_strings(const char* str1, const char* str2);

#endif // !SHARE_H