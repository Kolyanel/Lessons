#ifndef FUNC_HELPER_H
#define FUNC_HELPER_H

#include <stddef.h>
#include "data_func.h"

// функция ввода строки
void input_str(char *str, size_t len);

// подгоняем автоматически размер массива
Info *resize(Info *arr, size_t *size, size_t *cnt);

// функция сборки структуры
void input_data (Info *data);

// печать структуры
void print_data(const Info *data);

/* Функция создает динамически расширяемую строку
* Строка имеет запас места под расширение для имен файлов
* После использования сделать free
*/
char *read_line(FILE *ptr);

// чистка динамически выделеной памяти
void free_file(char **bin, char ** txt);

#endif //FUNC_HELPER_H