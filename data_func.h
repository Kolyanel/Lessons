#ifndef DATA_FUNC_H
#define DATA_FUNC_H

#include <stdbool.h>

#define NAME 32
#define MIN_SIZE 10


typedef struct{
	char fname[NAME];
	char sname[NAME];
	char lname[NAME];
} Name;

typedef struct{
	long num;
	Name person;
} Info;

extern Info *data;

// Динамическое выделение памяти под массив структур
Info *add_arr_data(size_t start);

// Собираем базу данных, массив структур
Info *create_data(Info *arr, size_t *size, size_t *cnt);

// Печать массива структур
void print_arr_data(const Info *arr, size_t cnt);

// Функция ввода имени файла с автоматическим приклеиванием расширения файла
void input_file(char **str1, char **str2);

// Функция записи бинарного файла массива структур
bool write_bin(Info *arr, char *file, size_t cnt);

// Функция записи текстового файла массива структур
bool write_txt(Info *arr, char *file, size_t cnt);

// Функция чтения файла с выводом на экран
void print_txt(const char *str);

// чтение бинарного файла
Info *read_bin(const char *file, size_t *cnt);

// Меню базы данных
void show_menu(void);

// движок программы
void select_menu(char **file_bin, char **file_txt, size_t *size_arr, size_t *cnt_person, int *run);

#endif //DATA_FUNC_H