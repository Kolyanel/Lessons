#include <stdio.h>

#define NAME 20
#define SIZE 10

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

void input_data (Info *data);

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

void input_data (Info *data)
{
	char *fn = data->person.fname;
	char *sn = data->person.sname;
	char *ln = data->person.lname;
	printf("Введите № карты социального страхования или 0 для завершения ввода: ");
	while(scanf("%ld", &data->num) != 1 || data->num < 0){
		fprintf(stderr, "Некорректно введен номер карты. Попробуйте еще раз: ");
		while(getchar() != '\n')
		    ;
	}
	while(getchar() != '\n')
		;
		if(data->num == 0){
			puts("Ввод окончен!");
			return;
		}
	printf("Введите имя клиента: ");
	input_str(fn, NAME);
	
	printf("Введите отчество клиента: ");
	input_str(sn, NAME);
	
	printf("Введите фамилию клиента: ");
	input_str(ln, NAME);
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