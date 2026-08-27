#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "calculations.h"
#include "file.h"
#include "types.h"
#include "queue.h"
#include "memory.h"

int main(void)
{
	Period date = {0};
	Data data = {0};
	Calc res = {0};
	Queue q;
	FILE *ft = NULL;
	FILE *fb = NULL;
	Indicator status;
	int ret = 0;
	size_t size_full_path = 0;
	
	char *full_path = path_alloc(&size_full_path);
	
	if (!full_path){
		errno = ENOMEM;
		perror("path_alloc");
		return 1;
	}
	
	const char *home = getenv("HOME");
	
	if (!home){
		home = "/";
	}
	
	if (creat_dir(DIR, home, full_path, size_full_path, S_IRUSR | S_IWUSR | S_IXUSR) != 0){
		fprintf(stderr, "Не удалось создать базу для программы: %s!\n", strerror(errno));
		free(full_path);
		return 1;
	}
	
	if (chdir(full_path) != 0){
		perror("chdir");
		free(full_path);
		return 1;
	}
	
	init_queue(&q);
	
	printf("Введите дату начала подсчета (дд.мм)\n");
	while(true){
		int ch;
		int n = scanf("%d.%d", &date.day, &date.month);
		if(n != 2 || date.day < 1 || date.day > 31 || date.month < 1 || date.month > 12){
			fputs("Некорректая дата. Попробуй снова\n", stderr);
			while((ch = getchar()) != '\n' && ch != EOF);
			continue;
			} else break;
	}
	
	status = open_file(date.day, &ft, &fb);
	if(status == ERR){
		fputs("Ошибка в работе файлов\n", stderr);
		free(full_path);
		return 1;
	}
	
	while(true){

		printf("Введите данные за %02d число или нечисловое значение для выхода\n", date.day);
		int n, ch;
		if(scanf("%d", &n) != 1) break;
		while((ch = getchar()) != '\n' && ch != EOF);
		
		data = create_data(date);
		if(!enqueue(&q, data)){
			fputs("Не удалось создать очередь\n", stderr);
			
			ret = 1;
			goto cleanup;
		}
		if(++date.day > 31){
		  date.day = 1;
		  if(++date.month > 12)
		  	date.month = 1;
		  }
	}
	if(!write_file(&q, ft, fb)){
		perror("Ошибка записи файлов");
		ret = 1;
		goto cleanup;
	}
	
cleanup:
	if(ft) fclose(ft);
	if(fb) fclose(fb);
	ft = fb = NULL;
	free(full_path);
	free_queue(&q);
	
	read_txt();
	read_bin_calc(&res);
	print_res(res);
	
	puts("Программа завершена!\n\nВсего доброго!");
	
	return ret;
}