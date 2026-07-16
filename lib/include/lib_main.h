/*
Заголовочный файл библиотеки для вывода hex-дампа файлов.
Теплых Семён Сергеевич
МК-101
*/
#ifndef LIB_MAIN_H
#define LIB_MAIN_H

#define MAX_CHUNK_SIZE 256
#define MAX_CHUNKS_PER_LINE 64

typedef struct {
    long offset;
    long size;
    int chunk_size;
    int chunks_per_line;
} HexParams;

void hex_params_init(HexParams* params);
int is_valid_number(const char* str);
int process_file(const char* filename, const HexParams* params);
int process_directory(const char* dir_path, const HexParams* params);

#endif