#ifndef DATA_FUNC_H
#define DATA_FUNC_H

#define NAME 20
#define SIZE 10

typedef struct{
	char fname[NAME];
	char sname[NAME];
	char lname[NAME];
} Name;

typedef struct{
	long num;
	Name person;
} Info;

// функция сборки структуры
void input_data (Info *data);

// печать структуры
void print_data(const Info *data);

#endif //DATA_FUNC_H