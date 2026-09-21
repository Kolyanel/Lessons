#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>
#include <errno.h>

#include "func.h"
#include "cpu.h"

#define DIR "my_deamon"

#define FILE_LOG "log_device.txt"

#define BAT_CONDITION "bat_condition.dat"

#define ERR_LOG "err.log"

static volatile sig_atomic_t stop;

static void sig_usr1(int signo) {(void) signo;}

static void sig_alrm(int signo) {(void) signo;}

static void sig_term(int signo)
{
	(void) signo;
	stop = 1;
}


int main(void)
{
	setlocale(LC_TIME, "ru_RU.UTF-8");
	
	pid_t pid;
	int fd;
	int bat_fd;
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
		_exit(EXIT_SUCCESS);
	
	if (setsid() < 0){
		write_err("Ошибка setsid");
		exit(EXIT_FAILURE);
	}
	
	if ((pid = fork()) < 0){
		write_err("Ошибка fork2");
		exit(EXIT_FAILURE);
	}
	
	if (pid > 0)
		_exit(EXIT_SUCCESS);
	
	int devnull = open("/dev/null", O_RDWR);
	
	if (devnull < 0){
		write_err("Не удалось открыть \"/dev/null\"");
		exit(EXIT_FAILURE);
	}
	
	if (dup2(devnull, STDIN_FILENO) < 0 || dup2(devnull, STDOUT_FILENO) < 0 || dup2(devnull, STDERR_FILENO) < 0){
		write_err("Ошибка dup2");
		close(devnull);
		exit(EXIT_FAILURE);
	}
	
	if (devnull > STDERR_FILENO)
		close(devnull);
	
	sigset_t set, oset;
	sigemptyset(&set);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGUSR1);
	
	sigprocmask(SIG_BLOCK, &set, &oset);
	
	struct sigaction sa;
	
	sa.sa_handler = sig_term;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	if (sigaction(SIGTERM, &sa, NULL) < 0){
		prerr_log(ERR_LOG, "Не удалось установить обработчик для SIGTERM");
	}
	
	sa.sa_handler = sig_usr1;
	if (sigaction(SIGUSR1, &sa, NULL) < 0){
		prerr_log(ERR_LOG, "Не удалось установить обработчик для SIGUSR1");
	}
	
	sa.sa_handler = sig_alrm;
	if (sigaction(SIGALRM, &sa, NULL) < 0){
		prerr_log(ERR_LOG, "Не удалось установить обработчик для SIGALRM");
	}
	
	dev_status_t device_status = {0};
	bat_status_t battery_status;
	
	while(1){
		
		if ((fd = open(FILE_LOG, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) < 0){
			alarm(10);
			sigsuspend(&oset);
			if (stop)
				break;
			continue;
		}
		
		if ((bat_fd = open(BAT_CONDITION, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0){
			close(fd);
			alarm(10);
			sigsuspend(&oset);
			if (stop)
				break;
			continue;
		}
		
		ssize_t n = read_fd_bin(bat_fd, &battery_status, sizeof(battery_status));
		
		if (n == 0){
			init_bat_status(&battery_status);
		} else if (n < 0){
			prerr_log(ERR_LOG, "Не удается прочитать сохраненное состояние батареи");
			close(fd);
			close(bat_fd);
			alarm(10);
			sigsuspend(&oset);
			if (stop)
				break;
			continue;
		} else if (n != sizeof(battery_status)){
			prerr_log(ERR_LOG, "Файл состояния батареи поврежден");
			close(fd);
			close(bat_fd);
			alarm(10);
			sigsuspend(&oset);
			if (stop)
				break;
			continue;
		}
		
		device_status.time_dev = time_now();
		if (!device_status.time_dev){
			prerr_log(ERR_LOG, "Не удается установить дату и время");
			close(fd);
			close(bat_fd);
			alarm(10);
			sigsuspend(&oset);
			if (stop)
				break;
			continue;
		}
		print_fd(fd, device_status.time_dev);
		
		device_status.load_cpu = get_workload_cpu();
		if (device_status.load_cpu >= 0){
			print_fd_workload_cpu(fd, device_status.load_cpu);
		} else{
			prerr_log(ERR_LOG, "Ошибка получения загруженности ЦПУ");
		}
		
		device_status.temp_cpu = get_cpu_temp();
		
		if (device_status.temp_cpu >= 0){
			print_fd_temp_cpu(fd, device_status.temp_cpu);
		} else{
			prerr_log(ERR_LOG, "Ошибка получения температуры ЦПУ");
		}
		
		update_battery_info(fd, &battery_status);
		
		if (lseek(bat_fd, 0, SEEK_SET) < 0){
			prerr_log(ERR_LOG, "Ошибка lseek");
			close(fd);
			close(bat_fd);
			free(device_status.time_dev);
			device_status.time_dev = NULL;
			alarm(5);
			sigsuspend(&oset);
			if (stop)
				break;
			continue;
		}
		
		ssize_t w = write(bat_fd, &battery_status, sizeof(battery_status));
		
		if (w != sizeof(battery_status)){
			prerr_log(ERR_LOG, "Ошибка сохранения накопленного состояния батареи");
			close(fd);
			close(bat_fd);
			free(device_status.time_dev);
			device_status.time_dev = NULL;
			alarm(10);
			sigsuspend(&oset);
			if (stop)
				break;
			continue;
		}
		
		free(device_status.time_dev);
		device_status.time_dev = NULL;
		close(fd);
		close(bat_fd);
		
		if (stop)
			break;
		
		alarm(3600);
		sigsuspend(&oset);
	}
	sigprocmask(SIG_SETMASK, &oset, NULL);
	
	exit(EXIT_SUCCESS);
}