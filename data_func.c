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
void input_file(char **str1, char **str2)
{
	printf("Введите имя файла: ");
	const char *exten[] = {".bin", ".txt"};
	size_t len;
	while(1){
		*str1 = read_line(stdin);
		if(!*str1){
			perror("Ошибка");
			return;
		}
		len = strlen(*str1);
		bool only_space = true;
		for(size_t i = 0; i < len; ++i){
			if(!isspace((unsigned char) (*str1)[i])){
				only_space = false;
				break;
			}
		}
		if(!only_space)
			break;
		free(*str1);
		printf("Имя файла не может быть пустым. Введите имя файла еще раз: ");
	}
	size_t need = len + strlen(exten[0]) + 1;
	char *tmp = realloc(*str1, need);
	if(!tmp){
		perror("Ошибка расширения памяти");
		free(*str1);
		*str1 = NULL;
		return;
	}
	*str1 = tmp;
	
	*str2 = malloc(need);
	if(!*str2){
		perror("Ошибка памяти для файла");
		free(*str1);
		*str1 = NULL;
		return;
	}
	strcpy(*str2, *str1);
	strcat(*str1, exten[0]);
	strcat(*str2, exten[1]);
}

// Функция записи бинарного файла массива структур
bool write_bin(Info *arr, char *file, size_t cnt)
{
	FILE *fp;
	if(!(fp = fopen(file, "ab"))){
		perror("Ошибка открытия файла для записи");
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
	if(!(fp = fopen(file, "w"))){
		perror("Ошибка открытия файла для звписи");
		return false;
	}
	if(fputs("===БАЗА ДАННЫХ===\n", fp) == EOF){
			perror("Ошибка записи заголовка");
			fclose(fp);
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
void print_txt(const char *str)
{
	FILE *fp;
	if(!str){
		perror("Ошибка чтения имени файла");
		return;
	}
	if(!(fp = fopen(str, "r"))){
		perror("Ошибка открытия файла");
		return;
	}
	char *buf;
	
	while((buf = read_line(fp)) ){
		puts(buf);
		free(buf);
	}
	if(ferror(fp))
		perror("Ошибка чтения файла");
	fclose(fp);
}

// чтение бинарного файла
Info *read_bin(const char *file, size_t *cnt)
{
	if(!cnt || !file || *file == '\0'){
		fputs("Некорректные параметры", stderr);
		return NULL;
	}
	FILE *fp = fopen(file, "rb");
	if(!fp){
		perror("Ошибка открытия файла");
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	if(size <= 0 || size % sizeof(Info) != 0){
		fputs("Файл пустой или поврежден\n", stderr);
		fclose(fp);
		return NULL;
	}
	*cnt = size / sizeof(Info);
	rewind(fp);
	Info *arr = add_arr_data(*cnt);
	if(!arr){
		fclose(fp);
		return NULL;
	}
	if(fread(arr, sizeof(Info), *cnt, fp) != *cnt){
		perror("Ошибка чтения файла");
		free(arr);
		fclose(fp);
		return NULL;
	}
	if(fclose(fp) == EOF)
		perror("Файл закрылся с ошибкой");
	return arr;
}

// Меню базы данных
void show_menu(void)
{
	puts("Выберите необходимую операцию:");
	puts("1. Создать новую;\n"
	     "2. Открыть имеющуюся;\n"
	     "3. Удалить;\n"
	     "0. Выйти");
}

// движок программы
void select_menu(char **file_bin, char **file_txt, size_t *size_arr, size_t *cnt_person)
{
	int n;
	printf("Сделайте выбор: ");
	if (scanf("%d", &n) != 1)
	{
		puts("Некорректный ввод");
		while (getchar() != '\n')
			;
		n = -1;
	}
	else
		while (getchar() != '\n')
			;

	switch (n)
	{
	case 1:
	{
		free_file(file_bin, file_txt);
		Info *data = add_arr_data(MIN_SIZE);
		if (!data)
		{
			perror("Ошибка выделения памяти");
			exit(EXIT_FAILURE);
		}
		input_file(file_bin, file_txt);
		if (!*file_bin || !*file_txt)
		{
			perror("Ошибка создания имени файла");
			free(data);
			exit(EXIT_FAILURE);
		}
		Info *tmp = create_data(data, size_arr, cnt_person);
		if (!tmp)
		{
			perror("База данных не создана");
			free(data);
			exit(EXIT_FAILURE);
		}
		data = tmp;
		if (!write_bin(data, *file_bin, *cnt_person))
		{
			perror("Ошибка записи в файл");
			free(data);
			exit(EXIT_FAILURE);
		}
		if (!write_txt(data, *file_txt, *cnt_person))
		{
			perror("Ошибка записи в файл");
			free(data);
			exit(EXIT_FAILURE);
		}
		print_txt(*file_txt);
		free(data);
		break;
	}
	case 2:
	{
		free_file(file_bin, file_txt);

		printf("Введите имя открываемого файла: ");
		input_file(file_bin, file_txt);
		if (!*file_bin || !*file_txt)
		{
			perror("Ошибка создания имени файла");
			break;
		}
		Info *data = read_bin(*file_bin, cnt_person);
		if (!data)
		{
			perror("Ошибка чтения файла");
			break;
		}
		print_txt(*file_txt);
		
		free(data);
	}
	break;
	case 3:
	{
	printf("Введите имя удаляемой базы данных: ");
	input_file(file_bin, file_txt);
	if (!*file_bin || !*file_txt)
		{
			perror("Ошибка создания имени файла");
			break;
		}
	int ok_bin = 0;
	int ok_txt = 0;
	if(*file_bin){
		ok_bin = (remove(*file_bin) == 0);
	}
	if(*file_txt){
		ok_txt = (remove(*file_txt) == 0);
	}
	if(ok_bin || ok_txt){
		puts("База данных удалена");
		free_file(file_bin, file_txt);
		} else {
			puts("База данных отсутствует");
		}
	}
	break;
	case 0:
	puts("Выход из программы!");
	free_file(file_bin, file_txt);
	exit(EXIT_SUCCESS);
	default:
	puts("Неверный выбор. Попробуйте еще раз");
	break;
	}
}