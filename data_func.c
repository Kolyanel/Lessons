#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

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
	puts("Заполните, преложенные ниже, поля");
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

// Функция ввода имени файла с автоматическим приклеиванием расширения файла
char *input_file(void)
{
	printf("Введите имя файла: ");
	char *str;
	const char *exten = ".bin";
	size_t len;
	while(1){
		str = read_line(stdin);
		if(!str){
			perror("Ошибка");
			return NULL;
		}
		len = strlen(str);
		bool only_space = true;
		for(size_t i = 0; i < len; ++i){
			if(!isspace((unsigned char) str[i])){
				only_space = false;
				break;
			}
		}
		if(!only_space)
			break;
		free(str);
		printf("Имя файла не может быть пустым. Введите имя файла еще раз: ");
	}
	size_t need = len + strlen(exten) + 1;
	char *tmp = realloc(str, need);
	if(!tmp){
		perror("Ошибка расширения памяти");
		free(str);
		return NULL;
	}
	str = tmp;
	strcat(str, exten);
	return str;
}

// Функция записи бинарного файла массива структур
bool write_bin(Info *arr, char *file, size_t cnt)
{
	FILE *fp;
	if(!(fp = fopen(file, "ab"))){
		perror("Ошибка открытия файла для звписи");
		return false;
	}
	
	if(fwrite(arr, sizeof(Info), cnt, fp) != cnt){
		perror("Ошибка записи в файл");
		fclose(fp);
		return false;
	}
	fclose(fp);
	return true;
}

// Функция записи текстового файла массива структур
bool write_txt(Info *arr, char *file, size_t cnt)
{
	FILE *fp;
	if(!(fp = fopen(file, "a"))){
		perror("Ошибка открытия файла для звписи");
		return false;
	}
	
	for(size_t i = 0; i < cnt; ++i){
		if(fprintf(fp, "%s %s %s - %ld\n", arr[i].person.lname, arr[i].person.fname, arr[i].person.sname, arr[i].num) < 0){
			perror("Ошибка записи текстового файла");
			fclose(fp);
			return false;
		}
	}
	fclose(fp);
	return true;
}

// Функция чтения файла с выводом на экран
void print_txt(void)
{
	FILE *fp;
	char *file;
	if(!(file = input_file())){
		perror("Ошибка чтения имени файла");
		return;
	}
	if(!(fp = fopen(file, "r"))){
		perror("Ошибка открытия файла");
		free(file);
		return;
	}
	char *buf;
	
	while((buf = read_line(fp)) ){
		puts(buf);
		free(buf);
	}
	if(ferror(fp))
		perror("Ошибка чтения файла");
	free(file);
	fclose(fp);
}