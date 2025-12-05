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
	
	size_t cnt_person = 0;
	size_t size_arr = MIN_SIZE;
	
	puts("===БАЗА ДАННЫХ КЛИЕНТОВ===");
	puts("Заполните, преложенные ниже, поля");
	
	data = create_data(data, &size_arr, &cnt_person);
	if(!data){
		perror("База данных не создана");
		exit(EXIT_FAILURE);
	}
	
	print_arr_data(data, cnt_person);

	free(data);
	return 0;
}