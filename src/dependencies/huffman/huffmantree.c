#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"
#include "../minheap/minheap.h"


struct min_heap_node* build_huffman_tree(int freq_table[], int size) {
    struct min_heap_node *left, *right, *top;
    struct min_heap* min_heap = create_min_heap(size);
    for (int i = 0; i < MAX_FREQ_TABLE_SIZE; i++) {
        if (freq_table[i] > 0) {
            insert_min_heap(min_heap, new_node((wchar_t)i, freq_table[i]));
        }
    }

    while (!is_size_one(min_heap)) {
        left = extract_min(min_heap);
        right = extract_min(min_heap);
        top = new_node(L'$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insert_min_heap(min_heap, top);
    }

    struct min_heap_node* huffmanTreeRoot = extract_min(min_heap);

    free(min_heap->array);
    free(min_heap);

    return huffmanTreeRoot;
}

void free_huffman_tree(struct min_heap_node* root) {
    if (root == NULL) {
        return;
    }

    free_huffman_tree(root->left);
    free_huffman_tree(root->right);

    free(root);
}


int calculate_freq_table_size(int freq_table[]) {
    int size = 0; 
    for (int i = 0; i < MAX_FREQ_TABLE_SIZE; i++) {
        if (freq_table[i] > 0) {
            size++;
        }
    }
    return size;
}
