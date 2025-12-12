#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "data_func.h"
#include "func_helper.h"

Info *data = NULL;

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
	puts("Заполните, предложенные ниже, поля");
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
	puts("\n====== УЧЕТ КЛИЕНТОВ ======\n");
	for(size_t i = 0; i < cnt; ++i){
		print_data(&arr[i]);
		if(i < cnt - 1)
			putchar('\n');
	}
	printf("\n==== КОНЕЦ БАЗЫ ДАННЫХ ====\n");
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
	if(!(fp = fopen(file, "wb"))){
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
		perror("Ошибка открытия файла для записи");
		return false;
	}
	if(fputs("\n======БАЗА ДАННЫХ======\n", fp) == EOF){
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
	if(fputs("\n===== КОНЕЦ ФАЙЛА ======\n", fp) == EOF){
			perror("Ошибка записи строки");
			fclose(fp);
			return false;
	}
	fclose(fp);
	return true;
}

// Функция чтения текстового файла с выводом на экран
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
void select_menu(char **file_bin, char **file_txt, size_t *size_arr, size_t *cnt_person, int *run){
	int n;
	printf("Сделайте выбор: ");
	if (scanf("%d", &n) != 1){
		puts("Некорректный ввод");
		while (getchar() != '\n');
		n = -1;
	} else
		while (getchar() != '\n');

	switch (n){
	case 1:
{
    int err_code = 0;
    
    // Очистка предыдущих данных
    free_file(file_bin, file_txt);
    free(data);
    data = NULL;
    *cnt_person = 0;
    *size_arr = MIN_SIZE;

    // Создание нового массива
    data = add_arr_data(MIN_SIZE);
    if (!data) {
        perror("Ошибка выделения памяти");
        exit(EXIT_FAILURE);
    }

    // Ввод имен файлов
    input_file(file_bin, file_txt);
    if (!*file_bin || !*file_txt) {
        perror("Ошибка создания имени файла");
        err_code = 1;
        goto cleanup;
    }

    // Создание данных
    data = create_data(data, size_arr, cnt_person);
    if (!data || *cnt_person == 0) {
        fputs("База данных пустая или не создана\n", stderr);
        err_code = 2;
        goto cleanup;
    }

    // Запись в файлы
    if (!write_bin(data, *file_bin, *cnt_person)) {
        perror("Ошибка записи в бинарный файл");
        err_code = 3;
        goto cleanup;
    }
    
    if (!write_txt(data, *file_txt, *cnt_person)) {
        perror("Ошибка записи в текстовый файл");
        err_code = 4;
        goto cleanup;
    }

    // Успешное завершение
    print_txt(*file_txt);
    puts("База данных успешно создана!");
    break;  // Выход при успехе

cleanup:
    // Очистка памяти (только если data не NULL)
    if (data) {
        free(data);
        data = NULL;
    }

    // Удаление файлов при ошибках записи
    if (err_code == 3) {
        // Ошибка записи бинарного файла
        if (*file_bin) remove(*file_bin);
        // Текстовый файл еще не создавался
    }
    else if (err_code == 4) {
        // Ошибка записи текстового файла
        if (*file_bin) remove(*file_bin);  // Удаляем уже созданный бинарный
        if (*file_txt) remove(*file_txt);  // Пытаемся удалить текстовый
    }

    // Очищаем имена файлов (всегда)
    free_file(file_bin, file_txt);
    }
    break;

			case 2:
{
//	printf("Введите имя открываемого файла: ");
	input_file(file_bin, file_txt);
	if (!*file_bin || !*file_txt)
	{
		perror("Ошибка создания имени файла");
		break;
	}
	
	print_txt(*file_txt);
	
	data = read_bin(*file_bin, cnt_person);
	if(!data){
		fputs("Ошибка чтения файла\n", stderr);
		break;
	}
	*size_arr = *cnt_person;
	
		puts("Выберите действия с открытым файлом:");
	while (1){
		puts("1. Добавить клиента;\n"
			 "2. Найти и редактировать клиента;\n"
			 "3. Сортировать по фамилии;\n"
			 "4. Сортировать по номеру;\n"
			 "0. Для выхода");
		int n;
		printf("Сделайте выбор: ");
		if (scanf("%d", &n) != 1){
			puts("Некорректный ввод");
			while (getchar() != '\n');
			n = -1;
		} else
			while (getchar() != '\n');
		
		char str[NAME] = "";

		switch (n) {
		case 1:
		data = create_data(data, size_arr, cnt_person);
		if(!data){
			perror("Ошибка добавления клиента");
			break;
		}
		break;
		
		case 2: {
    printf("Введите Фамилию или номер клиента: ");
    do { input_str(str, NAME);
    } while (str[0] == '\0' || str[0] == '\n');

    size_t pos = 0;
    bool found_any = false;

    while(pos < *cnt_person){
        if (strcasestr(data[pos].person.lname, str) ||
            (is_num(str) && data[pos].num == atol(str))) {

            print_data(&data[pos]);
            found_any = true;

            printf("\n[e] Редактировать, \n[d] Удалить клиента, \n[n] Следующий, [q] Выход: ");
            char choice = getchar();
            while(getchar() != '\n'); // очищаем stdin

            if(choice == 'e' || choice == 'E') {
                edit_data(&data[pos]);
                pos++;
                continue; // редактирование завершено
            } else if(choice == 'd' || choice == 'D'){
            	remove_pos(data, cnt_person, pos);
            	if(pos > 0) pos--;
            	continue;
            } else if(choice == 'n' || choice == 'N') {
                pos++; // идем к следующему совпадению
                continue;
            } else if(choice == 'q' || choice == 'Q') {
                break; // выход из поиска
            } else {
                puts("Неверный выбор, продолжаем поиск...");
                pos++;
                continue;
            }
        } else {
            pos++;
        }
    }

    if(!found_any){
        puts("Клиент не найден!");
        }
    }
    break;
		
		case 3:
		qsort(data, *cnt_person, sizeof(Info), cmp_str);
		print_arr_data(data, *cnt_person);
		break;
		
		case 4:
		qsort(data, *cnt_person, sizeof(Info), cmp_num);
		print_arr_data(data, *cnt_person);
		break;
		
		case 0:
		if(write_bin(data, *file_bin, *cnt_person) == 0){
			fputs("Ошибка записи файла\n", stderr);
			break;
		}
		if(write_txt(data, *file_txt, *cnt_person) == 0){
			fputs("Ошибка записи файла\n", stderr);
			break;
		}
		puts("Выход из подменю файла!");
		goto quit;
		
		default:
		puts("Неверный выбор. Попробуйте еще раз");
		break;
		}
	}
	quit:
	free_file(file_bin, file_txt);
	}
	break;
	
	case 3:
	{
	puts("=== УДАЛЕНИЕ БАЗЫ ДАННЫХ ===");
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
	free(data);
	data = NULL;
	*run = 0;
	break;
	
	default:
	puts("Неверный выбор. Попробуйте еще раз");
	break;
	}
}