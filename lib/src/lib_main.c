/*
lib_main.c - главный модуль библиотеки. 

Теплых Семён Сергеевич 
МК-101
*/

#include "lib_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

const char hex_chars[] = "0123456789ABCDEF";
int process_file(const char* filename, long offset, long size, int chunk_size, int chunks_per_line) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: failed to open file %s\n", filename);
        return -1;
    }
    // перемещение на нужное смещение
    if (fseek(f, offset, SEEK_SET) != 0) {
        fprintf(stderr, "Error: failed to navigate to offset %ld\n", offset);
        fclose(f);
        return -1;
    }
    // пока чтение строками по 16 байт 
    unsigned char buffer[16];
    long  file_pos = offset;
    long to_read = size;

    while (1) {
        long bytes_to_read = 16;
        if (size >= 0 && to_read < bytes_to_read) {
            bytes_to_read = to_read;
        }
        if (bytes_to_read <= 0) break;

        size_t read_bytes = fread(buffer, 1, bytes_to_read, f);
        if (read_bytes == 0) break;
        printf("%08lx  ", file_pos);
        // вывод байтов в hex
        for (size_t i = 0; i < read_bytes; i++) {
            unsigned char b = buffer[i];
            // перевод байта в два hex-символа тз
            printf("%c%c ", hex_chars[(b >> 4) & 0x0F], hex_chars[b & 0x0F]);
        }
        printf("| ");
        for (size_t i = 0; i < read_bytes; i++) {
            if (isprint(buffer[i])) {
                printf("%c", buffer[i]);
            }
            else {
                printf("*");
            }
        }
        printf("\n");
        file_pos += read_bytes;
        if (size >= 0) {
            to_read -= read_bytes;
        }
        if (read_bytes < bytes_to_read) break;
    }
    fclose(f);
    return 0;
}





