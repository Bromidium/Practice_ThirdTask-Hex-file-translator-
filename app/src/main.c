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

    // Флаги для отслеживания повторного использования опций
    int i_used = 0, d_used = 0, o_used = 0, l_used = 0, g_used = 0, n_used = 0;

    HexParams params;
    hex_params_init(&params);

    int opt;
    while ((opt = getopt(argc, argv, "i:o:l:g:n:d:h")) != -1) {
        switch (opt) {
            case 'i':
                if (i_used) { fprintf(stderr, "Error: -i specified multiple times\n"); return 1; }
                i_used = 1;
                input_file = optarg;
                break;
            case 'd':
                if (d_used) { fprintf(stderr, "Error: -d specified multiple times\n"); return 1; }
                d_used = 1;
                dir_path = optarg;
                break;
            case 'o':
                if (o_used) { fprintf(stderr, "Error: -o specified multiple times\n"); return 1; }
                if (!is_valid_number(optarg)) { fprintf(stderr, "Error: invalid number for -o\n"); return 1; }
                o_used = 1;
                params.offset = atol(optarg);
                break;
            case 'l':
                if (l_used) { fprintf(stderr, "Error: -l specified multiple times\n"); return 1; }
                if (!is_valid_number(optarg)) { fprintf(stderr, "Error: invalid number for -l\n"); return 1; }
                l_used = 1;
                params.size = atol(optarg);
                break;
            case 'g':
                if (g_used) { fprintf(stderr, "Error: -g specified multiple times\n"); return 1; }
                if (!is_valid_number(optarg)) { fprintf(stderr, "Error: invalid number for -g\n"); return 1; }
                g_used = 1;
                params.chunk_size = atoi(optarg);
                break;
            case 'n':
                if (n_used) { fprintf(stderr, "Error: -n specified multiple times\n"); return 1; }
                if (!is_valid_number(optarg)) { fprintf(stderr, "Error: invalid number for -n\n"); return 1; }
                n_used = 1;
                params.chunks_per_line = atoi(optarg);
                break;
            case 'h': print_usage(); return 0;
            default:
                fprintf(stderr, "Unknown option\n");
                print_usage();
                return 1;
        }
    }

    if (input_file && dir_path) {
        fprintf(stderr, "Error: cannot use both -i and -d\n");
        return 1;
    }
    if (!input_file && !dir_path) {
        fprintf(stderr, "Error: specify -i or -d\n");
        print_usage();
        return 1;
    }

    if (params.offset < 0) {
        fprintf(stderr, "Error: offset (-o) must be non-negative\n");
        return 1;
    }
    if (params.size < -1) {
        fprintf(stderr, "Error: size (-l) must be >= -1\n");
        return 1;
    }
    if (params.chunk_size < 1 || params.chunks_per_line < 1) {
        fprintf(stderr, "Error: invalid arguments\n");
        return 1;
    }

    // Защита от переполнения статических массивов
    if (params.chunk_size > MAX_CHUNK_SIZE) {
        fprintf(stderr, "Error: chunk size (-g) exceeds maximum (%d)\n", MAX_CHUNK_SIZE);
        return 1;
    }
    if (params.chunks_per_line > MAX_CHUNKS_PER_LINE) {
        fprintf(stderr, "Error: chunks per line (-n) exceeds maximum (%d)\n", MAX_CHUNKS_PER_LINE);
        return 1;
    }

    if (input_file) return process_file(input_file, &params);
    else return process_directory(dir_path, &params);
}