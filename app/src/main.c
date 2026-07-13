/*
main.c - главный модуль программы. 

Теплых Семён Сергеевич
МК-101
*/

#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"
#include "lib_main.h"

void print_help(void) {
    printf("  -i FILE   name input file\n");
    printf("  -d DIR    directory to crawl\n");
    printf("  -o OFFSET offset from the beginning of the file (default: 0)\n");
    printf("  -l SIZE   number of bytes to output (default: all)\n");
    printf("  -g SIZE   chunk size in bytes (default: 1)\n");
    printf("  -n COUNT  number of pieces per line (default: 16)\n");
    printf("  -h        help\n");
}

int main(int argc, char** argv) {
    char* input = NULL;
    char* dir = NULL;
    long offset = 0;
    long size = -1; // -1 значит вывести все
    int chunk_size = 1;
    int width = 16;
    int opt;
    // cтрока опций для getopt, буква и двоеточие - опция требует аргумент
    while ((opt = getopt(argc, argv, "i:o:l:g:n:d:h")) != -1) {
        switch (opt) {
        case 'i':
            input = optarg;
            break;
        case 'o':
            offset = strtol(optarg, NULL, 10);
            break;
        case 'l':
            size = strtol(optarg, NULL, 10);
            break;
        case 'g':
            chunk_size = strtol(optarg, NULL, 10);
            break;
        case 'n':
            width = strtol(optarg, NULL, 10);
            break;
        case 'd':
            dir = optarg;
            break;
        case 'h':
            print_help();
            return 0;
        default:
            fprintf(stderr, "Error: uknown option\n");
            return 1;
        }
    }
    // проверка на несовмесимость флагов
    if (input && dir) {
        fprintf(stderr, "Error: cannot specify both -i and -d at the same time\n");
        print_help();
        return 1;
    }
    // валидация числовых значений
    if (offset < 0) {
        fprintf(stderr, "Error: -o offset must be >= 0\n");
        return 1;
    }
    if (size < -1) {
        fprintf(stderr, "Error: -l must be >= -1\n");
        return 1;
    }
    if (chunk_size < 1) {
        fprintf(stderr, "Error: -g must be >= 1\n");
        return 1;
    }
    if (width < 1) {
        fprintf(stderr, "Error: -n must be >= 1\n");
        return 1;
    }

    // запуск обработки
    if (input) {
        return process_file(input, offset, size, chunk_size, width);
    }
    else if (dir) {
        return process_directory(dir, offset, size, chunk_size, width);
    }
    else {
        fprintf(stderr, "specify -i or -d\n");
        print_help();
        return 1;
    }

    

}
