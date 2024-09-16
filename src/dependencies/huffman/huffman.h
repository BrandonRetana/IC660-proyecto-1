#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "../global/structs.h"
#include "../global/vars.h"

void char_frequencies(wchar_t *buffer, int freq_table[]);

struct min_heap_node* new_node(wchar_t data, unsigned freq);

struct min_heap* create_min_heap(unsigned capacity);

struct min_heap_node* build_huffman_tree(int freq_table[], int size);

struct min_heap_node* extract_min(struct min_heap* min_heap);

void swap_min_heap_node(struct min_heap_node** a, struct min_heap_node** b);

void min_heapify(struct min_heap* min_heap, int idx);

void insert_min_heap(struct min_heap* min_heap, struct min_heap_node* min_heap_node);

void generate_huffman_codes(struct min_heap_node* node, int bits[], int idx, struct huffman_code* huffman_codes[]);

void free_huffman_tree(struct min_heap_node* root);

int is_size_one(struct min_heap* min_heap);

int calculate_freq_table_size(int freq_table[]);



#endif // !HUFFMAN_H