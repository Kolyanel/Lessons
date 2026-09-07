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
	
	if ((pid = fork()) < 0){
		write_err("Ошибка fork2");
		exit(EXIT_FAILURE);
	}
	
	if (pid > 0)
		exit(EXIT_SUCCESS);
	
	int devnull = open("/dev/null", O_RDWR);
	dup2(devnull, STDIN_FILENO);
	dup2(devnull, STDOUT_FILENO);
	dup2(devnull, STDERR_FILENO);
	
	if (devnull > STDERR_FILENO)
		close(devnull);
	
	dev_status_t device_status = {0};
	bat_status_t battery_status;
	
	init_bat_status(&battery_status);
	
	while(1){
		
		if ((fd = open("log_device.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) < 0){
			sleep(10);
			continue;
		}
		
		device_status.time_dev = time_now();
		if (!device_status.time_dev){
			print_fd(fd, "Не удается установить дату и время");
			close(fd);
			sleep(10);
			continue;
		}
		print_fd(fd, device_status.time_dev);
		
		device_status.load_cpu = get_workload_cpu();
		if (device_status.load_cpu >= 0){
			print_fd_workload_cpu(fd, device_status.load_cpu);
		} else{
			print_fd(fd, "Ошибка получения загруженности ЦПУ");
		}
		
		device_status.temp_cpu = get_cpu_temp();
		
		if (device_status.temp_cpu >= 0){
			print_fd_temp_cpu(fd, device_status.temp_cpu);
		} else{
			print_fd(fd, "Ошибка получения температуры ЦПУ");
		}
		
		update_battery_info(fd, &battery_status);
		
		free(device_status.time_dev);
		device_status.time_dev = NULL;
		close(fd);
		sleep(3600);
	}
	exit(EXIT_SUCCESS);
}