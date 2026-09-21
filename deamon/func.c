#include <unistd.h>
#include <stdio.h>
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



ssize_t read_fd_bin(int fd, void *dst, size_t size_dst)
{
	if (fd < 0 || !size_dst || !dst){
		errno = EINVAL;
		return -1;
	}
	
	char *p = dst;
	size_t total = 0;
	
	while(total < size_dst){
		
		ssize_t n = read(fd, p + total, size_dst - total);
		
		if (n > 0){
			total += (size_t) n;
			continue;
		}
		if (n < 0){
			
			if (errno == EINTR)
				continue;
			
			return -1;
		}
		
		// n == 0
		break;
	}
	return (ssize_t) total;
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



void prerr_log(const char *path, const char *msg)
{
	if (!path){
		errno = EINVAL;
		return;
	}
	
	if (!msg){
		msg = " неизвестная ошибка";
	}
	
	int fd_err;
	const char *str = "Дата и время не вычислены";
	
	char *time_log = time_now();
	if (!time_log)
		time_log = str;
	
	char buf[1024];
	
	snprintf(buf, sizeof(buf), "%s - %s", time_log, msg);
	
	if ((fd_err = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) < 0){
		if (time_log != str)
			free(time_log);
		return;
	}
	
	write(fd_err, buf, strlen(buf));
	write(fd_err, "\n", 1);
	
	close(fd_err);
	if (time_log != str)
		free(time_log);
}


void write_err(const char *msg)
{
	if (!msg){
		return;
	}
	write(STDERR_FILENO, msg, strlen(msg));
	write(STDERR_FILENO, "\n", 1);
}