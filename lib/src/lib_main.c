/*
Реализация библиотеки для вывода hex-дампа файлов.
Теплых Семён Сергеевич
МК-101
*/
#include "lib_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

// Максимальные размеры для статических массивов (защита от переполнения стека)
#define MAX_CHUNK_SIZE      256
#define MAX_CHUNKS_PER_LINE 64

// Преобразование байта в два шестнадцатеричных символа (заглавные)
static void byte_to_hex(unsigned char byte, char out[2]) {
    static const char table[] = "0123456789ABCDEF";
    out[0] = table[(byte >> 4) & 0x0F];
    out[1] = table[byte & 0x0F];
}

// Проверка, является ли байт печатным ASCII-символом
static int is_printable_ascii(unsigned char c) {
    return (c >= 0x20 && c <= 0x7E);
}

// Вывод одной строки дампа
static void print_dump_line(long offset,
                            unsigned char chunks[][MAX_CHUNK_SIZE],
                            int chunk_sizes[],
                            int num_chunks,
                            const HexParams* params)
{
    printf("%08lx  ", offset);

    // Выводим все chunks_per_line позиций (даже если файл закончился, добиваем пробелами)
    for (int i = 0; i < params->chunks_per_line; i++) {
        if (i < num_chunks) {
            // Выводим байты кусочка в обратном порядке (little-endian)
            for (int j = params->chunk_size - 1; j >= 0; j--) {
                unsigned char b = (j < chunk_sizes[i]) ? chunks[i][j] : 0;
                char hex[2];
                byte_to_hex(b, hex);
                putchar(hex[0]);
                putchar(hex[1]);
            }
        } else {
            // Паддинг пробелами для недостающих кусочков, чтобы столбцы не ехали
            for (int j = 0; j < params->chunk_size * 2; j++) {
                putchar(' ');
            }
        }
        putchar(' '); // Разделитель между кусочками
    }

    // ASCII-представление только если chunk_size == 1
    if (params->chunk_size == 1) {
        printf("| ");
        for (int i = 0; i < num_chunks; i++) {
            unsigned char c = chunks[i][0];
            putchar(is_printable_ascii(c) ? c : '.');
        }
    }
    putchar('\n');
}

int is_valid_number(const char* str) {
    if (*str == '-') str++;
    if (*str == '\0') return 0;
    while (*str) {
        if (*str < '0' || *str > '9') return 0;
        str++;
    }
    return 1;
}

void hex_params_init(HexParams* params) {
    params->offset = 0;
    params->size = -1;
    params->chunk_size = 1;
    params->chunks_per_line = 16;
}

int process_file(const char* filename, const HexParams* params) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror(filename);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    if (params->offset >= file_size) {
        fclose(f);
        return 0;
    }
    fseek(f, params->offset, SEEK_SET);

    long bytes_to_read = params->size;
    if (bytes_to_read < 0 || params->offset + bytes_to_read > file_size) {
        bytes_to_read = file_size - params->offset;
    }

    long current_offset = params->offset;

    // Статические массивы вместо malloc (безопаснее и быстрее)
    unsigned char chunks[MAX_CHUNKS_PER_LINE][MAX_CHUNK_SIZE];
    int chunk_sizes[MAX_CHUNKS_PER_LINE];

    while (bytes_to_read > 0) {
        int num_chunks = 0;
        long line_bytes = 0;

        for (int i = 0; i < params->chunks_per_line && bytes_to_read > 0; i++) {
            int want = params->chunk_size;
            if (want > bytes_to_read) want = (int)bytes_to_read;

            int actual = (int)fread(chunks[num_chunks], 1, want, f);
            if (actual <= 0) break;

            chunk_sizes[num_chunks] = actual;
            num_chunks++;
            bytes_to_read -= actual;
            line_bytes += actual;
        }

        if (num_chunks == 0) break;

        print_dump_line(current_offset, chunks, chunk_sizes, num_chunks, params);
        current_offset += line_bytes;
    }

    fclose(f);
    return 0;
}

int process_directory(const char* dir_path, const HexParams* params) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        perror(dir_path);
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
            printf("\n--- File: %s ---\n", full_path);
            process_file(full_path, params);
        }
    }

    closedir(dir);
    return 0;
}