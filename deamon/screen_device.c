#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <locale.h>
#include <errno.h>

#include "func.h"
#include "cpu.h"

#define DIR "my_deamon"


int main(void)
{
	setlocale(LC_TIME, "ru_RU.UTF-8");
	
	pid_t pid;
	int fd;
	struct stat st;
	umask(0);
	
	const char *home = getenv("HOME");
	if (!home)
		home = "/";
	
	if (chdir(home) != 0){
		write_err("Не удалось перейти в папку");
			exit(EXIT_FAILURE);
	}
	
	if (stat(DIR, &st) == 0){
		if (!S_ISDIR(st.st_mode)){
			write_err("Не папка");
			exit(EXIT_FAILURE);
		}
	} else {
		if (errno == ENOENT){
			if (mkdir(DIR, S_IRUSR | S_IWUSR | S_IXUSR) != 0){
				write_err("Не удалось создать папку для программы");
				exit(EXIT_FAILURE);
			}
		} else{
			write_err("Ошибка stat");
			exit(EXIT_FAILURE);
		}
	}
	
	if (chdir(DIR) != 0){
		write_err("Не удалось перейти");
			exit(EXIT_FAILURE);
	}
	
	if ((pid = fork()) < 0){
		write_err("Ошибка fork");
		exit(EXIT_FAILURE);
	}
	
	if (pid > 0)
		exit(EXIT_SUCCESS);
	
	if (setsid() < 0){
		write_err("Ошибка setsid");
		exit(EXIT_FAILURE);
	}
	
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	while(1){
		
		if ((fd = open("log_device.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) < 0){
			//print_fd(fd, "Не удалась работа с логфайлом");
			sleep(10);
			continue;
		}
		
		char *time = time_now();
		if (!time){
			print_fd(fd, "Не удается установить дату и время");
			close(fd);
			sleep(10);
			continue;
		}
		print_fd(fd, time);
		
		double load = get_workload_cpu();
		if (load >= 0){
			print_fd_workload_cpu(fd, load);
		} else{
			print_fd(fd, "Ошибка получения загруженности ЦПУ");
		}
		
		double temp = get_cpu_temp();
		
		if (temp >= 0){
			print_fd_temp_cpu(fd, temp);
		} else{
			print_fd(fd, "Ошибка получения температуры ЦПУ");
		}
		
		update_battery_info(fd);
		
		free(time);
		close(fd);
		sleep(3600);
	}
	exit(EXIT_SUCCESS);
}