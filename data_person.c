#include <stdio.h>

#define <data_func.h>

typedef struct{
	char fname[NAME];
	char sname[NAME];
	char lname[NAME];
} Name;

typedef struct{
	long num;
	Name person;
} Info;

void input_str(char *str, size_t len);

void print_data(const Info *data);

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
		fputs("Вы не ввели текст. Поле стается пустым\n", stderr);
	}
}

void print_data(const Info *data)
{
	char *fn = data->person.fname;
	char *sn = data->person.sname;
	char *ln = data->person.lname;
	if(sn[0] != '\0'){
		printf("%s, %.2s %.2s. - %ld\n", ln, fn, sn, data->num);
	} else{
		printf("%s, %s - %ld\n", ln, fn, data->num);
	}
}