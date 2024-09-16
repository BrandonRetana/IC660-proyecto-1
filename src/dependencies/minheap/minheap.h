#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "../global/structs.h"
#include "../global/vars.h"

struct min_heap* create_min_heap(unsigned capacity);
void insert_min_heap(struct min_heap* min_heap, struct min_heap_node* min_heap_node);
struct min_heap_node* extract_min(struct min_heap* min_heap);
void min_heapify(struct min_heap* min_heap, int idx);
int is_size_one(struct min_heap* min_heap);
struct min_heap_node* new_node(wchar_t data, unsigned freq);
#endif // MIN_HEAP_H
