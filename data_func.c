#include <stdio.h>
#include <stdlib.h>

#include "data_func.h"
#include "func_helper.h"

// Динамическое выделение памяти под массив структур
Info *add_arr_data(size_t start)
{
	Info *arr = malloc(start * sizeof(Info));
	if(!arr){
		perror("Ошибка выделения памяти");
		return NULL;
	}
	return arr;
}

// Собираем базу данных, массив структур
Info *create_data(Info *arr, size_t *size, size_t *cnt)
{
	if(!arr){
		fputs("База данных не создана\n", stderr);
		return NULL;
	}
	while(1){
		if(*cnt >= *size){
			arr = resize(arr, size, cnt);
			if(!arr){
				perror("Нет базы данных");
				return NULL;
			}
		}
		Info tmp;
		input_data(&tmp);
		if(tmp.num <= 0)
			break;
		arr[*cnt] = tmp;
		(*cnt)++;
	}
	if (*cnt == 0)
		puts("Ничего не добавлено.");
	return arr;
}

// Печать массива структур
void print_arr_data(const Info *arr, size_t cnt)
{
	if(!arr || cnt == 0){
		puts("База данных пуста или не создана");
		return;
	}
	for(size_t i = 0; i < cnt; ++i){
		print_data(&arr[i]);
		if(i < cnt - 1)
			putchar('\n');
	}
	printf("\n=== КОНЕЦ БАЗЫ ДАННЫХ ===\n");
}