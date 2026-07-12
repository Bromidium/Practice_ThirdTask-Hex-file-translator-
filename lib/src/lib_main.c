/*
Реализация функций библиотеки для hex-дампа.
Теплых Семён Сергеевич
МК-101
*/

#include "lib_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> 

const char hex_chars[] = "0123456789ABCDEF";
int process_file(const char* filename, long offset, long size, int chunk_size, int chunks_per_line) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Ошибка: не удалось открыть файл %s\n", filename);
        return -1;
    }

    if (fseek(f, offset, SEEK_SET) != 0) {
        fprintf(stderr, "Ошибка: не удалось перейти к смещению %ld\n", offset);
        fclose(f);
        return -1;
    }
    // память под строку и под один кусочек
    int bytes_per_line = chunk_size * chunks_per_line;
    unsigned char* buffer = (unsigned char*)malloc(bytes_per_line);
    unsigned char* chunk_buf = (unsigned char*)malloc(chunk_size);
    if (!buffer || !chunk_buf) {
        fprintf(stderr, "Ошибка: не удалось выделить память\n");
        fclose(f);
        free(buffer);
        free(chunk_buf);
        return -1;
    }
    long current_offset = offset;
    long bytes_left = size;
    while (1) {
        long bytes_to_read = bytes_per_line;
        if (size >= 0 && bytes_left < bytes_to_read) {
            bytes_to_read = bytes_left;
        }
        if (bytes_to_read <= 0) break;

        size_t read_bytes = fread(buffer, 1, bytes_to_read, f);
        if (read_bytes == 0) break;
        printf("%08lx  ", current_offset);
        // вывод кусочков
        for (int i = 0; i < chunks_per_line; i++) {
            int chunk_start = i * chunk_size;

            if (chunk_start >= read_bytes) {
                // файл закончился, добиваем пробелами для выравнивания столбцов
                for (int k = 0; k < chunk_size * 2; k++) printf(" ");
                printf(" ");
                continue;
            }

            int actual = chunk_size;
            if (chunk_start + actual > read_bytes) {
                actual = read_bytes - chunk_start;
            }

            // заполнение chunk_buf нулями, копируем реальные байты в начало
            memset(chunk_buf, 0, chunk_size);
            memcpy(chunk_buf, &buffer[chunk_start], actual);

            // байты кусочка в little-endian
            for (int j = chunk_size - 1; j >= 0; j--) {
                unsigned char b = chunk_buf[j];
                printf("%c%c", hex_chars[(b >> 4) & 0x0F], hex_chars[b & 0x0F]);
            }
            printf(" "); 
        }
        if (chunk_size == 1) {
            printf("| ");
            for (size_t i = 0; i < read_bytes; i++) {
                if (isprint(buffer[i])) {
                    printf("%c", buffer[i]);
                }
                else {
                    printf("*");
                }
            }
        }
        printf("\n");

        current_offset += read_bytes;
        if (size >= 0) {
            bytes_left -= read_bytes;
        }

        if (read_bytes < bytes_to_read) break; 
    }

    free(buffer);
    free(chunk_buf);
    fclose(f);
    return 0;
}