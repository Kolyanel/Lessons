#include <stdio.h>
#include <stdlib.h>

#include "data_func.h"

int main(void)
{
/*	Info *data = add_arr_data(MIN_SIZE);
	if(!data){
		perror("Ошибка выделения памяти");
		exit(EXIT_FAILURE);
	}*/
	
	char *file_bin = NULL;
	char *file_txt = NULL;
	size_t cnt_person = 0;
	size_t size_arr = MIN_SIZE;
	
	puts("===БАЗА ДАННЫХ КЛИЕНТОВ===");
	while(1){
		show_menu();
		select_menu(&file_bin, &file_txt, &size_arr, &cnt_person);
	}
/*	Info *tmp = create_data(data, &size_arr, &cnt_person);
	if(!tmp){
		perror("База данных не создана");
		free(data);
		exit(EXIT_FAILURE);
	}
	data = tmp;
	
	input_file(&file_bin, &file_txt);
	if(!file_bin || !file_txt){
		perror("Ошибка создания имени файла");
		free(data);
		free(file_bin);
		free(file_txt);
		exit(EXIT_FAILURE);
	}
	
	if(!write_bin(data, file_bin, cnt_person)){
		perror("Ошибка записи в файл");
		free(data);
		free(file_bin);
		free(file_txt);
		exit(EXIT_FAILURE);
	}
	
	if(!write_txt(data, file_txt, cnt_person)){
		perror("Ошибка записи в файл");
		free(data);
		free(file_bin);
		free(file_txt);
		exit(EXIT_FAILURE);
	}
	
	print_txt(file_txt);

	free(data);
	free(file_bin);
	free(file_txt);
	*/
	return 0;
}