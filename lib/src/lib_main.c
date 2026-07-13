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
#ifdef _WIN32
#include <windows.h>
#endif

const char hex_chars[] = "0123456789ABCDEF";

static void to_hex(unsigned char byte, char* out) {
    out[0] = hex_chars[(byte >> 4) & 0x0F];
    out[1] = hex_chars[byte & 0x0F];
    out[2] = '\0';
}

static char get_printable(unsigned char byte) {
    if (isprint(byte)) {
        return (char)byte;
    }
    return '*';
}

int process_file(const char* filename, long offset, long size, int chunk_size, int chunks_per_line) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: failed to open file %s\n", filename);
        return -1;
    }

    if (fseek(f, offset, SEEK_SET) != 0) {
        fprintf(stderr, "Error: failed to navigate to offset %ld\n", offset);
        fclose(f);
        return -1;
    }
    // память под строку и под один кусочек
    int bytes_per_line = chunk_size * chunks_per_line;
    unsigned char* buffer = (unsigned char*)malloc(bytes_per_line);
    unsigned char* chunk_buf = (unsigned char*)malloc(chunk_size);
    if (!buffer || !chunk_buf) {
        fprintf(stderr, "Error: failed to allocate memory\n");
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
        if (read_bytes == 0) {
            if (ferror(f)) {
                fprintf(stderr, "Error: read error in file %s\n", filename);
                free(buffer);
                free(chunk_buf);
                fclose(f);
                return -1;
            }
            break;
        }
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
                char hex[3];
                to_hex(chunk_buf[j], hex);
                printf("%s", hex);
            }
            printf(" "); 
        }
        if (chunk_size == 1) {
            printf("| ");
            for (size_t i = 0; i < read_bytes; i++) {
                printf("%c", get_printable(buffer[i]));
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

// обход директории
int process_directory(const char* dir, long offset, long size, int chunk_size, int width) {
#ifdef _WIN32
    // путь для поиска всех файлов в папке
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", dir);

    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error: failed to open directory %s\n", dir);
        return -1;
    }
    // цикл по всем найденным элементам
    do {
        // пропуск директори1, только файлы
        if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char file_path[MAX_PATH];
            snprintf(file_path, MAX_PATH, "%s\\%s", dir, find_data.cFileName);
            printf("file: %s\n", file_path);
            process_file(file_path, offset, size, chunk_size, width);
        }
    } while (FindNextFileA(hFind, &find_data));

    FindClose(hFind);
    return 0;
#else
    // если вдруг на Linux
    fprintf(stderr, "Windows only\n");
    return -1;
#endif
}