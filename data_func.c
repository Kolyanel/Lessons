#include <stdio.h>

#include "data_func.h"
#include "func_helper.h"

// функция сборки структуры
void input_data (Info *data)
{
	char *fn = data->person.fname;
	char *sn = data->person.sname;
	char *ln = data->person.lname;
	printf("Введите № карты социального страхования или 0 для завершения ввода: ");
	while(scanf("%ld", &data->num) != 1 || data->num < 0){
		fprintf(stderr, "Некорректно введен номер карты. Попробуйте еще раз: ");
		while(getchar() != '\n')
		    ;
	}
	while(getchar() != '\n')
		;
		if(data->num == 0){
			puts("Ввод окончен!");
			return;
		}
	printf("Введите имя клиента: ");
	input_str(fn, NAME);
	
	printf("Введите отчество клиента: ");
	input_str(sn, NAME);
	
	printf("Введите фамилию клиента: ");
	input_str(ln, NAME);
}

// печать структуры
void print_data(const Info *data)
{
	const char *fn = data->person.fname;
	const char *sn = data->person.sname;
	const char *ln = data->person.lname;
	if(sn[0] != '\0'){
		printf("%s, %.2s. %.2s. - %ld\n", ln, fn, sn, data->num);
	} else{
		printf("%s, %s - %ld\n", ln, fn, data->num);
	}
}