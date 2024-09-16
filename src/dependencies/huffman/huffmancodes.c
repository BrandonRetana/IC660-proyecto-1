#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"


void generate_huffman_codes(struct min_heap_node* node, int bits[], int idx, struct huffman_code* huffman_codes[]) {
    if (node == NULL) {
        return;
    }
    if (node->left) {
        bits[idx] = 0;
        generate_huffman_codes(node->left, bits, idx + 1, huffman_codes);
    }

    if (node->right) {
        bits[idx] = 1;
        generate_huffman_codes(node->right, bits, idx + 1, huffman_codes);
    }

    if (!(node->left) && !(node->right)) {
        huffman_codes[node->data] = (struct huffman_code*)malloc(sizeof(struct huffman_code));
        if (huffman_codes[node->data] == NULL) {
            perror("La peticion de memoria fallo. No se puede reservar memoria para la estrcutura");
            exit(EXIT_FAILURE);
        }
        huffman_codes[node->data]->length = idx;
        for (int i = 0; i < idx; ++i) {
            huffman_codes[node->data]->code[i] = bits[i];
        }
    }
}

