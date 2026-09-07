#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>

#include "func.h"



char * read_path(const char *path, int flags)
{
	if (!path){
		errno = EINVAL;
		return NULL;
	}
	
	char *buf = malloc(BUF_SIZE);
	if (!buf)
		return NULL;
	
	int fd;
	ssize_t n;
	size_t len = 0;
	size_t cap = BUF_SIZE;
	
	if ((fd = open(path, flags)) < 0){
		free(buf);
		return NULL;
	}
	
	while(1){
		
		if (len == cap -1){
			if (cap > SIZE_MAX / 2){
				close(fd);
				free(buf);
				return NULL;
			}
			
			size_t new_cap = cap * 2;
			
			char *tmp = realloc(buf, new_cap);
			if (!tmp){
				close(fd);
				free(buf);
				return NULL;
			}
			
			buf = tmp;
			cap = new_cap;
		}
		
		n = read(fd, buf + len, cap - len - 1);
		
		if (n == 0)
			break;
		
		if (n < 0){
			if (errno == EINTR)
				continue;
			close(fd);
			free(buf);
			return NULL;
		}
		
		len += n;
		
	}
	
	close(fd);
	
	buf[len] = '\0';
	
	return buf;
}



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


void print_fd(int fd, const char *msg)
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
	if (!msg){
		return;
	}
	write(STDERR_FILENO, msg, strlen(msg));
	write(STDERR_FILENO, "\n", 1);
}