#ifndef FUNC_H
#define FUNC_H

#include <sys/types.h>
#include <stddef.h>

#ifndef BUF_SIZE
	#define BUF_SIZE 1024
#endif


/* читаем содержимое файла по указаному пути
* при ошибках возвращаем NULL
* при успехе указатель на массив с содержимым файла
* вызывающий код должен сделать free*/
char * read_path(const char *path, int flags);



/*
* читает любые бинарные данные из открытого fd
* заполняет поданый в нее указатель на буфер
* возвращает -1 при ошибке
* при удаче количество прочитаных байт
* может вернуть меньше чем size_dst
*/
ssize_t read_fd_bin(int fd, void *dst, size_t size_dst);


// получение текущего времени
char *time_now(void);


// печать сообщения в файловый дескриптор
void print_fd(int fd, const char *msg);


// печать ошибок в поток ошибок
void write_err(const char *msg);


#endif