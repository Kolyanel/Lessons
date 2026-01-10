#include <stdio.h>
#include <stdbool.h>

#include "calculations.h"

// считывает ввод пользователя и формирует данные за один день
Data create_data(Period d)
{
	Data data_day = {0};
	data_day.date = d;
	int x, sum_x = 0;
	int ch;
	printf("Введите социальные карты (-1 - для завершения ввода): ");
	while(true){
		if(scanf("%d", &x) != 1){
			fputs("Некорректное значение\n", stderr);
			while((ch = getchar()) != '\n' && ch != EOF);
			continue;
		}
		if(x == -1) break;
		sum_x += x;
	}
	data_day.socCard = (sum_x * PRICE_TICKET) / (float)PRICE_FUEL;
	
	sum_x = 0;
	printf("Введите банковские карты (-1 - для завершения ввода): ");
	while(true){
		if(scanf("%d", &x) != 1){
			fputs("Некорректное значение\n", stderr);
			while((ch = getchar()) != '\n' && ch != EOF);
			continue;
		}
		if(x == -1) break;
		sum_x += x;
	}
	data_day.bankCard = sum_x * PRICE_BANK_CARD;
	
	sum_x = 0;
	printf("Введите ЕГКС карты (-1 - для завершения ввода): ");
	while(true){
		if(scanf("%d", &x) != 1){
			fputs("Некорректное значение\n", stderr);
			while((ch = getchar()) != '\n' && ch != EOF);
			continue;
		}
		if(x == -1) break;
		
		if(x % PRICE_EGCS != 0){
			fputs("Ошибка ввода суммы ЕГКС\n", stderr);
			continue;
		}
		sum_x += x;
	}
	data_day.egcsCard = (sum_x / PRICE_EGCS) * PRICE_TICKET;
	
	return data_day;
}

// вывод общего результата
void print_res(Calc res)
{
	puts("\n====== ОБЩИЕ ДАННЫЕ ======");
	printf("Топлива - %.2f | Банк - %d | ЕГКС - %d\n", res.fuel_total, res.bank_total, res.egcs_total);
}