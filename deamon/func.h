#ifndef FUNC_H
#define FUNC_H

#ifndef BUF_SIZE
	#define BUF_SIZE 1024
#endif


/* читаем содержимое файла по указаному пути
* при ошибках возвращаем NULL
* при успехе указатель на массив с содержимым файла
* вызывающий код должен сделать free*/
char * read_path(const char *path, int flags);


// получение текущего времени
char *time_now(void);


// печать сообщения в файловый дескриптор
void print_fd(int fd, const char *msg);


// печать ошибок в поток ошибок
void write_err(const char *msg);


#endif