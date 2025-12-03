#include <stdio.h>

#include "data_func.h"

int main(void)
{
	Info data[SIZE] = {0};
	int cnt = 0;
	
	while(cnt < SIZE){
		input_data(&data[cnt]);
		if(data[cnt].num == 0)
			break;
		cnt++;
	}
	for(int i = 0; i < cnt; ++i){
		print_data(&data[i]);
	}
	return 0;
}
