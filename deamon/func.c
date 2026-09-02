#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "func.h"


char *time_now(void)
{
	time_t now = time(NULL);
	
	struct tm *tm_info = localtime(&now);
	
	if (!tm_info){
		errno = EINVAL;
		return NULL;
	}
	
	char *buf = malloc(128);
	if (!buf){
		errno = ENOMEM;
		return NULL;
	}
	
	if (!strftime(buf, 128, "\n%d-%B-%Y %H:%M:%S", tm_info)){
		free(buf);
		errno = EOVERFLOW;
		return NULL;
	}
	
	char *tmp = realloc(buf, strlen(buf) + 1);
	if (tmp){
		buf = tmp;
	}
	
	return buf;
}


void print_fd(int fd, char *msg)
{
	if (fd < 0 || !msg){
		errno = EINVAL;
		return;
	}
	
	write(fd, msg, strlen(msg));
	
	write(fd, "\n", 1);
}


void write_err(const char *msg)
{
	write(STDERR_FILENO, msg, strlen(msg));
	write(STDERR_FILENO, "\n", 1);
}