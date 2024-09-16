#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#include "../share/share.h"
#include "../huffman/huffman.h"

void char_frequencies(wchar_t *buffer, int freq_table[]) {
    if (buffer == NULL || freq_table == NULL) {
        fprintf(stderr, "Error: puntero nulo pasado a char_frequencies\n");
        return;
    }

    wchar_t *ptr = buffer;
    while (*ptr != L'\0') {
        wchar_t ch = *ptr;
        if (ch >= 0 && ch < CHAR_SET_SIZE) {
            freq_table[ch]++;
        } else {
            fprintf(stderr, "Advertencia: carácter fuera de rango encontrado: %lc\n", ch);
        }
        ptr++;
    }
}
