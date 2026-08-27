#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdbool.h>

#include "types.h"
#include "queue.h"

#define FILE_TXT "/storage/emulated/0/Coding/fare_cards.txt"

#define FILE_BIN "fare_cards.bin"

#define DIR "fare_data"

// открытие файлов для записи или дописывания информации
Indicator open_file(int day, FILE **ft, FILE **fb);

// запись информации в файлы
bool write_file(const Queue *q, FILE *ft, FILE *fb);

// чтение текстового файла и вывод на экран
void read_txt(void);

// чтение бинарного файла и получение общего результата
void read_bin_calc(Calc *res);


// создание рабочей папки для программы
int creat_dir(const char *name, const char *path, char* full_path, size_t size_full_path, int mode);

#endif //FILE_H