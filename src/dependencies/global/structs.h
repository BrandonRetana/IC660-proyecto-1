#ifndef STRUCTS_H
#define STRUCTS_H

#include "vars.h"

struct min_heap_node {
    wchar_t data;                
    unsigned freq;                
    struct min_heap_node *left;     
    struct min_heap_node *right;    
};

struct min_heap {
    unsigned size;              
    unsigned capacity;          
    struct min_heap_node **array; 
};

struct huffman_code {
    int code[MAX_CODE_SIZE];
    int length;
};


struct directory_metadata {
    const char* directory;
    int num_txt_files;
    size_t offsets[TOTAL_BOOKS];
};


struct encode_args {
    char books[TOTAL_BOOKS][MAX_NAME_SIZE];
    int file_count;
};


struct thread_args_c {
    char* bookFile;
    FILE *outputFile;
    int pos;
    size_t* offsets_ptr;
};

struct thread_args_d {
    FILE *binary_source;
    const char* outputPath;
    size_t offset;
    int pos;
};

#endif