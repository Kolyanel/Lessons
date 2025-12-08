#include <stdio.h>
#include <stdlib.h>

#include "data_func.h"

int main(void)
{
	
	char *file_bin = NULL;
	char *file_txt = NULL;
	size_t cnt_person = 0;
	size_t size_arr = MIN_SIZE;
	
	puts("===БАЗА ДАННЫХ КЛИЕНТОВ===");
	while(1){
		show_menu();
		select_menu(&file_bin, &file_txt, &size_arr, &cnt_person);
	}
	return 0;
}