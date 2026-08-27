#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "file.h"


// открытие файлов для записи или дописывания информации
Indicator open_file(int day, FILE **ft, FILE **fb)
{
	const char *mode_txt;
	const char *mode_bin;
	*ft = *fb = NULL;
	Indicator res;
	
	if(day == FIRST_DAY){
		mode_txt = "w";
		mode_bin = "wb";
		res = WRITE;
	} else{
		mode_txt = "a";
		mode_bin = "ab";
		res = APPEND;
	}
	if(!(*ft = fopen(FILE_TXT, mode_txt))){
		perror("Открытие txt");
		return ERR;
	}
	if(!(*fb = fopen(FILE_BIN, mode_bin))){
		perror("Открытие bin");
		fclose(*ft);
		return ERR;
	}
	return res;
}

// запись информации в файлы
bool write_file(const Queue *q, FILE *ft, FILE *fb)
{
	if(!q || !ft || !fb){
		return false;
	}
	
	const Node *cur = q->head;
	while(cur){
		const Data *d = &cur->data;
		
		if(fprintf(ft, "%02d.%02d - Топливо: %.2f | Банк: %d | ЕГКС: %d\n", d->date.day, d->date.month, d->socCard, d->bankCard, d->egcsCard) < 0){
			return false;
		}
		
		if(fwrite(d, sizeof(*d), 1, fb) != 1){
			return false;
		}
		cur = cur->next;
	}
	return true;
}



// чтение текстового файла и вывод на экран
void read_txt(void)
{
	FILE *ft = fopen(FILE_TXT, "r");
	if(!ft){
		perror("Ошибка открытия текстового файла");
		return;
	}
	puts("======= РАСЧЕТ КАРТ =======");
	char buf[BUF_SIZE] = "";
	while(fgets(buf,  BUF_SIZE, ft)){
		fputs(buf, stdout);
	}
	fclose(ft);
}

// чтение бинарного файла и получение общего результата
void read_bin_calc(Calc *res)
{
	if(!res){
		fputs("Место для хранения результатов не существует\n", stderr);
		return;
	}
	FILE *fb = fopen(FILE_BIN, "rb");
	if(!fb){
		perror("Ошибка открытия бинарного файла");
		return;
	}
	Data d;
	
	while(fread(&d, sizeof(d), 1, fb) == 1){
		res->fuel_total += d.socCard;
		res->bank_total += d.bankCard;
		res->egcs_total += d.egcsCard;
	}
	fclose(fb);
}


// создание рабочей папки для программы
int creat_dir(const char *name, const char *path, char* full_path, size_t size_full_path, int mode)
{
	if (!name || !path || !full_path){
		errno = EINVAL;
		return -1;
	}
	
	struct stat st;
	
	if (name[0] == '/')
		name++;
	
	snprintf(full_path, size_full_path, "%s/%s", path, name);
	
	if (stat(full_path, &st) == 0){
		if (S_ISDIR(st.st_mode))
			return 0;
		else{
			fputs("Это не папка!\n", stderr);
			return -1;
		}
	}
	
	if (errno == ENOENT){
		mode_t old_umask = umask(0);
		
		int res = mkdir(full_path, mode);
		
		umask(old_umask);
		
		if (res == 0){
			errno = 0;
			return 0;
		} else{
			perror("mkdir");
			return -1;
		}
	} else{
		perror("Ошибка проверки пути");
		return -1;
	}
}