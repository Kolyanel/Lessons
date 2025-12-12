#include <stdio.h>
#include <stdlib.h>

#include "data_func.h"
#include "func_helper.h"

int main(void)
{
	
	char *file_bin = NULL;
	char *file_txt = NULL;
	size_t cnt_person = 0;
	size_t size_arr = MIN_SIZE;
	int run = 1;
	
	puts("===БАЗА ДАННЫХ КЛИЕНТОВ===");
	while(run){
		show_menu();
		select_menu(&file_bin, &file_txt, &size_arr, &cnt_person, &run);
	}
	puts("\n==ПРОГРАММА ЗАВЕРШЕНА!!!==");
	free_file(&file_bin, &file_txt);
	free(data);
	return 0;
}