#include <stdio.h>
#include <string.h>

#include "func_helper.h"

// функция ввода строки
void input_str(char *str, size_t len)
{
	int ch;
	while(!fgets(str, len, stdin)){
		fputs("Неудалось ввести текст\n", stderr);
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
