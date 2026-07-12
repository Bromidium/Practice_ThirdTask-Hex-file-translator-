/*
lib_main.h - заголовочный файл модуля lib_main.c. 

Теплых Семён Сергеевич
МК-101
*/


#ifndef LIB_MAIN_H
#define LIB_MAIN_H

// обработка и вывод одного файла
// return 0 при успехе, -1 при ошибке
int process_file(const char* filename, long offset, long size, int chunk_size, int chunks_per_line);

#endif // LIB_MAIN_H
