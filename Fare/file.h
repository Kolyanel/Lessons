#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdbool.h>

#include "types.h"
#include "queue.h"

// открытие файлов для записи или дописывания информации
Indicator open_file(int day, FILE **ft, FILE **fb);

// запись информации в файлы
bool write_file(const Queue *q, FILE *ft, FILE *fb);

// чтение текстового файла и вывод на экран
void read_txt(void);

// чтение бинарного файла и получение общего результата
void read_bin_calc(Calc *res);

#endif //FILE_H