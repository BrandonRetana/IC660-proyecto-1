#ifndef VARS_H
#define VARS_H

#define MAX_FREQ_TABLE_SIZE 65536
#define MAX_CODE_SIZE 256
#define INITIAL_BUFFER 1024 
#define CHAR_SET_SIZE 65536
#define TOTAL_BOOKS 98 
#define MAX_NAME_SIZE 128 
#define MAX_PATH_LENGTH 128

#define BUFFER_SIZE 4096
#define TEMP_FILENAME_LEN 128
#define SHM_SIZE (TOTAL_BOOKS * sizeof(size_t))

#define OUTPUT_PATH "output/bin/compressed.bin"
#define DIRECTORY_PATH "output/"
#define TEMP_PATH "output/temp"
#define BOOKS_PATH "books"

#endif