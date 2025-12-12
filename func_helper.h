#ifndef FUNC_HELPER_H
#define FUNC_HELPER_H

#include <stddef.h>
#include <stdbool.h>
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

// функция определения строка это или число
bool is_num(const char *str);

// поиск в структуое по номеру
int find_by_num(const Info *arr, size_t cnt, long num);

// поиск в структуре по фамилии
int find_by_lname(const Info *arr, size_t cnt, const char *str);

// для сортировки по номеру
int cmp_num(const void *a, const void *b);

// для сортировки по фамилии
int cmp_str(const void *a, const void *b);

// поиск клиента в структуре
int find_person(const Info *arr, size_t cnt, const char *str);

// редоктирование данных клиента
void edit_data(Info *d);

//  удаляем одну строку
void remove_pos(Info *arr, size_t *cnt, size_t pos);

#endif //FUNC_HELPER_H