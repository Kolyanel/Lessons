#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>

#include "cpu.h"
#include "func.h"


int read_cpu_stats(cpu_stats_t *stats)
{
	int fd = open("/proc/stat", O_RDONLY);
	if (fd < 0){
		return -1;
	}
	
	char buf[1024];
	ssize_t n = read(fd, buf, sizeof(buf) - 1);
	if (n < 0){
		close(fd);
		return -1;
	}
	buf[n] = '\0';
	
	close(fd);
	
	int parsed = sscanf(buf, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", 
	&stats->user,
	&stats->nice,
	&stats->system,
	&stats->idle,
	&stats->iowait,
	&stats->irq,
	&stats->softirq,
	&stats->steal);
	
	return (parsed == 8) ? 0 : -1;
}


double get_cpu_usage(void)
{
	static cpu_stats_t prev;
	cpu_stats_t curr;
	static int first_call = true;
	
	if (read_cpu_stats(&curr) != 0)
		return -1.0;
	
	if (first_call){
		prev = curr;
		first_call = false;
		return 0.0;
	}
	
	ull prev_total = prev.user + prev.nice + prev.system + prev.idle + prev.iowait + prev.irq + prev.softirq + prev.steal;
	
	ull curr_total = curr.user + curr.nice + curr.system + curr.idle + curr.iowait + curr.irq + curr.softirq + curr.steal;
	
	ull prev_idle_total = prev.idle + prev.iowait;
	
	ull curr_idle_total = curr.idle + curr.iowait;
	
	ull total_diff = curr_total - prev_total;
	
	ull idle_diff = curr_idle_total - prev_idle_total;
	
	prev = curr;
	
	if (!total_diff)
		return 0.0;
	
	return 100 * (1.0 - (double) idle_diff / total_diff);
}



char *read_global_state(void)
{
	char *line = malloc(1024);
	if (!line)
		return NULL;
	
	int fd = open("/sys/devices/system/cpu/cpu0/core_ctl/global_state", O_RDONLY);
	
	if (fd < 0){
		free(line);
		return NULL;
	}
	
	ssize_t n = read(fd, line, 1023);
	if (n < 0){
		free(line);
		close(fd);
		return NULL;
	}
	
	close(fd);
	
	line[n] = '\0';
	
	return line;
}



double get_workload_cpu(void)
{
	int busy_buf[32];
	int cnt = 0;
	int total = 0;
	char *line = read_global_state();
	if (!line)
		return -1.0;
	
	char *p = line;
	
	while((p = strstr(p, "Busy%:")) && cnt < sizeof(busy_buf) / sizeof(int)){
		if (sscanf(p, "Busy%%: %d", &busy_buf[cnt]) == 1){
			total += busy_buf[cnt];
			cnt++;
		}
		p++;
	}
	free(line);
	
	if (!cnt)
		return -1.0;
	
	return (double) total / cnt;
}



void print_fd_workload_cpu(int fd, double load)
{
	char buf[64];
	snprintf(buf, sizeof(buf), "Загрузка ЦПУ = %.2f%%\n", load);
	write(fd, buf, strlen(buf));
}



double get_cpu_temp(void)
{
	int fd;
	const char *paths[] = {
		"/sys/class/thermal/thermal_zone23/temp",
        "/sys/class/thermal/thermal_zone22/temp",
        "/sys/class/thermal/thermal_zone21/temp",
        "/sys/class/thermal/thermal_zone20/temp",
        "/sys/class/thermal/thermal_zone19/temp",
        "/sys/class/thermal/thermal_zone18/temp",
        "/sys/class/thermal/thermal_zone17/temp",
        NULL
	};
	
	for(int i = 0; paths[i]; ++i){
		fd = open(paths[i], O_RDONLY);
		if (fd < 0)
			continue;
	
		char buf[32];
		ssize_t n = read(fd, buf, sizeof(buf) - 1);
		close(fd);
	
		if (n <= 0)
			continue;
	
		buf[n] = '\0';
	
		char *endptr;
	
		long val = strtol(buf, &endptr, 10);
	
		if (endptr == buf)
			continue;
	
		if (val > 0 && val < 120000){
			return val / 1000.0;
		}
	}
	
	return -1.0;
}



void print_fd_temp_cpu(int fd, double temp)
{
	char buf[64];
	snprintf(buf, sizeof(buf), "Температура ЦПУ = %.1fC\n", temp);
	write(fd, buf, strlen(buf));
}



void update_battery_info(int fd)
{
	static int start_capacity = -1;
	static long long start_counter = -1;
	
	char buf[64];
	int capacity;
	long long counter;
	
	int bat_fd = open("/sys/class/power_supply/battery/capacity", O_RDONLY);
	
	if (bat_fd < 0){
		print_fd(fd, "Ошибка чтения заряда батареи");
		return;
	}
	
	ssize_t n = read(bat_fd, buf, sizeof(buf) - 1);
	close(bat_fd);
	
	if (n <= 0){
		print_fd(fd, "Ошибка чтения capacity");
		return;
	}
	
	buf[n] = '\0';
	
	if (sscanf(buf, "%d", &capacity) != 1){
		print_fd(fd, "Ошибка разбора capacity");
		return;
	}
	
	
	bat_fd = open("/sys/class/power_supply/battery/charge_counter", O_RDONLY);
	
	if (bat_fd < 0){
		print_fd(fd, "Ошибка открытия charge_counter");
		return;
	}
	
	n = read(bat_fd, buf, sizeof(buf) - 1);
	close(bat_fd);
	
	if (n <= 0){
		print_fd(fd, "Ошибка чтения charge_counter");
		return;
	}
	
	buf[n] = '\0';
	
	if (sscanf(buf, "%lld", &counter) != 1){
		print_fd(fd, "Ошибка разбора charge_counter");
		return;
	}
	
	char logbuf[128];
	
	snprintf(logbuf, sizeof(logbuf), "Батарея = %d%%, остаток = %lld mAh\n", capacity, counter / 1000);
	
	print_fd(fd, logbuf);
	
	if (start_capacity < 0 || start_counter < 0){
		start_capacity = capacity;
		start_counter = counter;
		return;
	}
	
	if (capacity >= start_capacity || counter >= start_counter)
		return;
	
	int delta_capacity = start_capacity - capacity;
	
	long long delta_counter = start_counter - counter;
	
	if (delta_capacity < 5)
		return;
	
	double estimated_capacity = ((double) delta_counter / 1000.0) * 100 / (double) delta_capacity;
	
	const double design_capacity = 5000.0;
	
	double soh = estimated_capacity / design_capacity * 100.0;
	
	snprintf(logbuf, sizeof(logbuf), "Оценка полной емкости АКБ = %.0f mAh\n"
	"Оценка состояния АКБ = %.1f%%\n", estimated_capacity, soh);
	
	print_fd(fd, logbuf);
	
	start_capacity = capacity;
	start_counter = counter;
}