/*
lib_main.h - заголовочный файл модуля lib_main.c. 

Теплых Семён Сергеевич
МК-101
*/


#ifndef LIB_MAIN_H
#define LIB_MAIN_H

// обработка и вывод одного файла
int process_file(const char* filename, long offset, long size, int chunk_size, int width);
// обработка всех файлов в директории
int process_directory(const char* dir, long offset, long size, int chunk_size, int width);
#endif // LIB_MAIN_H
