#include <stdio.h>
#include <stdlib.h>

#include "data_func.h"

int main(void)
{
	Info *data = add_arr_data(MIN_SIZE);
	if(!data){
		perror("Ошибка выделения памяти");
		exit(EXIT_FAILURE);
	}
	
	char * file;
	size_t cnt_person = 0;
	size_t size_arr = MIN_SIZE;
	
	puts("===БАЗА ДАННЫХ КЛИЕНТОВ===");
	
	data = create_data(data, &size_arr, &cnt_person);
	if(!data){
		perror("База данных не создана");
		exit(EXIT_FAILURE);
	}
	
	file = input_file();
	if(!file){
		perror("Ошибка чтения имени файла");
		exit(EXIT_FAILURE);
	}
	
	if(!write_bin(data, file, cnt_person)){
		perror("Ошибка записи в файл");
		exit(EXIT_FAILURE);
	}
	free(file);
	
	file = input_file();
	if(!file){
		perror("Ошибка чтения имени файла");
		exit(EXIT_FAILURE);
	}
	
	if(!write_txt(data, file, cnt_person)){
		perror("Ошибка записи в файл");
		exit(EXIT_FAILURE);
	}
	
	print_txt();

	free(data);
	free(file);
	return 0;
}