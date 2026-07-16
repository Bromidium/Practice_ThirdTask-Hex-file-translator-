/*
Главный модуль. Разбор аргументов и запуск.
Теплых Семён Сергеевич
МК-101
*/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "lib_main.h"

static void print_usage(void) {
    printf("Usage: hexdump [options]\n");
    printf("  -i FILE   input file\n");
    printf("  -d DIR    directory\n");
    printf("  -o OFF    offset (default: 0)\n");
    printf("  -l SIZE   bytes to show (default: all)\n");
    printf("  -g SIZE   chunk size (default: 1)\n");
    printf("  -n COUNT  chunks per line (default: 16)\n");
    printf("  -h        help\n");
}

int main(int argc, char** argv) {
    char* input_file = NULL;
    char* dir_path = NULL;

    HexParams params;
    hex_params_init(&params);

    int opt;
    while ((opt = getopt(argc, argv, "i:o:l:g:n:d:h")) != -1) {
        switch (opt) {
            case 'i': input_file = optarg; break;
            case 'd': dir_path = optarg; break;
            case 'o': params.offset = atol(optarg); break;
            case 'l': params.size = atol(optarg); break;
            case 'g': params.chunk_size = atoi(optarg); break;
            case 'n': params.chunks_per_line = atoi(optarg); break;
            case 'h': print_usage(); return 0;
            default:
                fprintf(stderr, "Unknown option\n");
                print_usage();
                return 1;
        }
    }

    if (!input_file && !dir_path) {
        fprintf(stderr, "Error: specify -i or -d\n");
        print_usage();
        return 1;
    }

    if (input_file) return process_file(input_file, &params);
    else return process_directory(dir_path, &params);
}