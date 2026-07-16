/*
Реализация библиотеки для вывода hex-дампа файлов.
Теплых Семён Сергеевич
МК-101
*/
#include "lib_main.h"
#include <stdio.h>
#include <stdlib.h>

void hex_params_init(HexParams* params) {
    params->offset = 0;
    params->size = -1;
    params->chunk_size = 1;
    params->chunks_per_line = 16;
}

int is_valid_number(const char* str) { return 1; }

int process_file(const char* filename, const HexParams* params) { return 0; }

int process_directory(const char* dir_path, const HexParams* params) { return 0; }