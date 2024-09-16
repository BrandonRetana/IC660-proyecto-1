#include <stdio.h>
#include <stdlib.h>
#include "minheap.h"

struct min_heap_node* new_node(wchar_t data, unsigned freq) {
    struct min_heap_node* temp = (struct min_heap_node*)malloc(sizeof(struct min_heap_node));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}


struct min_heap* create_min_heap(unsigned capacity) {
    struct min_heap* min_heap = (struct min_heap*)malloc(sizeof(struct min_heap));
    min_heap->size = 0;
    min_heap->capacity = capacity;
    min_heap->array = (struct min_heap_node**)malloc(min_heap->capacity * sizeof(struct min_heap_node*));
    return min_heap;
}


void swap_min_heap_node(struct min_heap_node** a, struct min_heap_node** b) {
    struct min_heap_node* t = *a;
    *a = *b;
    *b = t;
}

void min_heapify(struct min_heap* min_heap, int idx) {
    int smallest = idx; 
    int left = 2 * idx + 1; 
    int right = 2 * idx + 2; 

    if (left < min_heap->size && min_heap->array[left]->freq < min_heap->array[smallest]->freq) {
        smallest = left;
    }

    if (right < min_heap->size && min_heap->array[right]->freq < min_heap->array[smallest]->freq) {
        smallest = right;
    }

    if (smallest != idx) {
        swap_min_heap_node(&min_heap->array[smallest], &min_heap->array[idx]); 
        min_heapify(min_heap, smallest);
    }
}

int is_size_one(struct min_heap* min_heap) {
    return (min_heap->size == 1);
}

struct min_heap_node* extract_min(struct min_heap* min_heap) {
    struct min_heap_node* temp = min_heap->array[0];
    min_heap->array[0] = min_heap->array[min_heap->size - 1];
    --min_heap->size;
    min_heapify(min_heap, 0);
    return temp;
}


void insert_min_heap(struct min_heap* min_heap, struct min_heap_node* min_heap_node) {
    ++min_heap->size; 
    int i = min_heap->size - 1;
    while (i && min_heap_node->freq < min_heap->array[(i - 1) / 2]->freq) {
        min_heap->array[i] = min_heap->array[(i - 1) / 2]; 
        i = (i - 1) / 2;  
    }
    min_heap->array[i] = min_heap_node;
}

