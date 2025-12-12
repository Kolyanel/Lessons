#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "data_func.h"
#include "func_helper.h"

// функция ввода строки
void input_str(char *str, size_t len)
{
	int ch;
	while(!fgets(str, len, stdin)){
		fputs("Не удалось ввести текст\n", stderr);
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
		while(getchar() != '\n');
		}
	while(getchar() != '\n');
	if(data->num == 0){
		puts("Ввод окончен!");
		return;
	}
	printf("Введите имя клиента: ");
	input_str(fn, NAME);
	
	printf("Введите отчество клиента: ");
	input_str(sn, NAME);
	
	printf("Введите фамилию клиента: ");
	while(1){
		input_str(ln, NAME);
		if(ln[0] != '\0' && ln[0] != '\n')
			break;
		else
			printf("Фамилия не может быть пустой! Введите новую фамилию: \n");
	}
}

// печать структуры
void print_data(const Info *data)
{
	const char *fn = data->person.fname;
	const char *sn = data->person.sname;
	const char *ln = data->person.lname;
	if(sn[0] != '\0'){
		printf("%s %s %s - %ld\n", ln, fn, sn, data->num);
	} else{
		printf("%s %s - %ld\n", ln, fn, data->num);
	}
}

/* Функция создает динамически расширяемую строку
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

// чистка динамически выделеной памяти
void free_file(char **bin, char ** txt)
{
	free(*bin);
	free(*txt);
	*bin = NULL;
	*txt = NULL;
}

// функция определения строка это или число
bool is_num(const char *str)
{
	if(!str || !*str)
		return false;
	for(int i = 0; str[i]; ++i){
		if(!isdigit((unsigned char) str[i]))
			return false;
	}
	return true;
}

// поиск в структуое по номеру
int find_by_num(const Info *arr, size_t cnt, long num)
{
	for(size_t i = 0; i < cnt; ++i){
		if(arr[i].num == num)
			return i;
	}
	return -1;
}

// поиск в структуре по фамилии
int find_by_lname(const Info *arr, size_t cnt, const char *str)
{
	if(!str || !*str)
		return -1;
	for(size_t i = 0; i < cnt; ++i){
		if(strcasestr(arr[i].person.lname, str))
			return i;
	}
	return -1;
}

// для сортировки по номеру
int cmp_num(const void *a, const void *b)
{
    const Info *x = a;
    const Info *y = b;
    if(x->num < y->num)
    	return -1;
    if(x->num > y->num)
    	return 1;
    return 0;
}

// для сортировки по фамилии
int cmp_str(const void *a, const void *b)
{
    const Info *x = a;
    const Info *y = b;
    return strcasecmp(x->person.lname, y->person.lname);
}

// поиск клиента в структуре
int find_person(const Info *arr, size_t cnt, const char *str)
{
	if(!arr || !str){
		fputs("Некорректно поданы данные!", stderr);
		return -1;
	}
	if(is_num(str)){
		long num = atol(str);
		return find_by_num(arr, cnt, num);
	} else
		return find_by_lname(arr, cnt, str);
}

// редоктирование данных клиента
void edit_data(Info *d)
{
	puts("======РЕДАКТИРОВАНИЕ======");
	int n = -1;
	while(n != 0){
		puts("1. Изменить имя;\n"
		     "2. Изменить отчество;\n"
		     "3. Изменить фамилию;\n"
		     "4. Изменить № карты соц. страхования;\n"
		     "0. Выход из редактирования.\n");
		 printf("Сделайте свой выбор: ");
		 if (scanf("%d", &n) != 1){
			puts("Некорректный ввод");
			while (getchar() != '\n');
			n = -1;
		} else
			while (getchar() != '\n');
		
		switch (n){
			case 1:
			printf("Введите новое имя: \n");
			input_str(d->person.fname, NAME);
			break;
			
			case 2:
			printf("Введите новое отчество: \n");
			input_str(d->person.sname, NAME);
			break;
			
			case 3:
			printf("Введите новую фамилию: \n");
			while(1){
				input_str(d->person.lname, NAME);
				if(d->person.lname[0] != '\0' && d->person.lname[0] != '\n')
					break;
				else
					printf("Фамилия не может быть пустой! Введите новую фамилию: \n");
			}
			break;
			
			case 4:
			printf("Введите новый № карты соц. страхования: ");
			while(scanf("%ld", &d->num) != 1 || d->num < 0){
				fprintf(stderr, "Некорректно введен номер карты. Попробуйте еще раз: ");
				while(getchar() != '\n');
				}
				while(getchar() != '\n');
				break;
				
				case 0:
				return;
				default:
				printf("Некорректное значение. Попробуйте еще раз: \n");
				break;
		} // switch
	} //внешний while
	puts("=== УСПЕШНОЕ РЕДАКТИРОВАНИЕ! ===");
}

//  удаляем одну строку
void remove_pos(Info *arr, size_t *cnt, size_t pos)
{
	if(!arr || !cnt || *cnt == 0){
		fputs("Каталога нет!\n", stderr);
		return;
	}
	if(pos < *cnt){
		for(size_t i = pos; i < *cnt - 1; ++i){
			arr[i] = arr[i + 1];
		}
		(*cnt)--;
	}
}