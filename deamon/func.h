#ifndef FUNC_H
#define FUNC_H


// получение текущего времени
char *time_now(void);


// печать сообщения в файловый дескриптор
void print_fd(int fd, char *msg);


// печать ошибок в поток ошибок
void write_err(const char *msg);


#endif