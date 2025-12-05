#ifndef DATA_FUNC_H
#define DATA_FUNC_H

#define NAME 20
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

// Динамическое выделение памяти под массив структур
Info *add_arr_data(size_t start);

// Собираем базу данных, массив структур
Info *create_data(Info *arr, size_t *size, size_t *cnt);

// Печать массива структур
void print_arr_data(const Info *arr, size_t cnt);

#endif //DATA_FUNC_H