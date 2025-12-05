#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "data_func.h"
#include "func_helper.h"

// функция ввода строки
void input_str(char *str, size_t len)
{
	int ch;
	while(!fgets(str, len, stdin)){
		fputs("Неудалось ввести текст\n", stderr);
		printf("Попробуйте еще раз: ");
	}
	size_t n = strcspn(str, "\n");
	if(str[n] == '\n')
		str[n] = '\0';
	else
		while((ch = getchar()) != '\n' && ch != EOF);
	if(str[0] == '\0'){
		fputs("Вы не ввели текст. Поле остается пустым\n", stderr);
	}
}

// подгоняем автоматически размер массива
Info *resize(Info *arr, size_t *size, size_t *cnt)
{
	if(!arr){
		fputs("База данных не создана\n", stderr);
		return NULL;
	}
	if(*cnt == 0){
		puts("Пустая база данных!");
		return arr;
	}

	if(*size == *cnt){
		size_t new_s = *size * 2;
		Info *tmp = realloc(arr, new_s * sizeof(Info));
		if(!tmp){
			perror("Не удалось увеличить базу данных! Она осталась прежней");
			return arr;
		}
		*size = new_s;
		arr = tmp;
	} else if(*cnt < (*size / 2) && *size > MIN_SIZE){
		size_t new_s = (*cnt < MIN_SIZE) ? MIN_SIZE : *cnt;
		Info *tmp = realloc(arr, new_s * sizeof(Info));
		if(!tmp){
			perror("Не удалось уменьшить размер базы данных! Она осталась прежней");
			return arr;
		}
		arr = tmp;
		*size = new_s;
	}
	return arr;
}

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

/* Функция создает динамически расширяемую строку
* Строка имеет запас места под расширение для имен файлов
* После использования сделать free
*/
char *read_line(FILE *ptr)
{
    char *str = NULL;
    char *tmp;
    size_t init = 64; // начальный размер буфера

    // Выделяем память
    if (!(str = (char *) malloc(init))) {
        perror("Ошибка выделения памяти для строки");
        return NULL;
    }

    // Читаем первые данные
    if (!fgets(str, init, ptr)) {
        if (!feof(ptr)) 
            fputs("Ошибка чтения строки\n", stderr);
        free(str);
        return NULL;
    }

    size_t len = strlen(str);

    // Расширяем буфер, если строка не уместилась
    while (len > 0 && str[len - 1] != '\n' && !feof(ptr)) {
        init *= 2;
        if (!(tmp = (char *) realloc(str, init))) {
            perror("Ошибка расширения памяти");
            free(str);
            return NULL;
        }
        str = tmp;
        
        if (!fgets(str + len, init - len, ptr)) {
            if(!feof(ptr)){
            	perror("Не удалось дописать строку");
  	          free(str);
     	       return NULL;
            }
            break;
        }

        len = strlen(str);
    }

    // Поджимаем память, если выделено слишком много
    if (len + 1 < init / 2) {
        if ((tmp = (char *) realloc(str, len + 1)))
            str = tmp;
    }

    // Убираем финальный '\n', если есть
    str[strcspn(str, "\r\n")] = '\0';

    // Проверка пустой строки для stdin
    if (ptr == stdin && str[0] == '\0') {
        fputs("Пустая строка\n", stderr);
        free(str);
        return NULL;
    }
    return str;
}