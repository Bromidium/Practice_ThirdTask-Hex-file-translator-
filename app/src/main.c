/*
main.c - главный модуль программы. 

Теплых Семён Сергеевич
МК-101
*/

#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"
#include "lib_main.h"

int main(int argc, char** argv) {
    char* input = NULL;
    char* dir = NULL;
    long offset = 0;
    long size = -1; // -1 значит вывести все
    int chunk_size = 1;
    int width = 16;
    int opt;
    // cтрока опций для getopt, буква и двоеточие - опция требует аргумент
    while ((opt = getopt(argc, argv, "i:o:l:g:n:d:")) != -1) {
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
        default:
            fprintf(stderr, "Error: uknown option\n");
            return 1;
        }
    }

    // для отладки, убрать
    if (input) printf("filr (-i): %s\n", input);
    if (dir) printf("dir (-d): %s\n", dir);
    printf("move (-o): %ld\n", offset);
    printf("size (-l): %ld\n", size);
    printf("piece size (-g): %d\n", chunk_size);
    printf("pieces in str (-n): %d\n", width);

    return 0;
}
