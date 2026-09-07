#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>

#include "cpu.h"
#include "func.h"


void init_bat_status(bat_status_t *bs)
{
	if (!bs){
		errno = EINVAL;
		return;
	}
	
	bs->state = BAT_UNKNOWN;
	bs->last_capacity = -1;
	bs->last_counter = -1;
	bs->total_capacity_delta = 0;
	bs->total_counter_delta = 0;
	bs->estimated_capacity = 0.0;
	bs->soh = 0.0;
}



void get_bat_status(bat_status_t *bs, const char *status)
{
	if (!bs || !status){
		errno = EINVAL;
		return;
	}
	
	if (strcmp(status, "Discharging") == 0){
		bs->state = BAT_DISCHARG;
		return;
	}
		
	if (strcmp(status, "Charging") == 0){
		bs->state = BAT_CHARG;
		return;
	}
		
	if (strcmp(status, "Full") == 0){
		bs->state = BAT_FULL;
		return;
	}
	
	if (strcmp(status, "Not charging") == 0){
		bs->state = BAT_NOT_CHARG;
		return;
	}
		
	bs->state = BAT_UNKNOWN;
}



int read_cpu_stats(cpu_stats_t *stats)
{
	if (!stats){
		errno = EINVAL;
		return -1;
	}
	
	char *buf = read_path(PROC_STAT, O_RDONLY);
	
	if (!buf)
		return -1;
	
	int parsed = sscanf(buf, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", 
	&stats->user,
	&stats->nice,
	&stats->system,
	&stats->idle,
	&stats->iowait,
	&stats->irq,
	&stats->softirq,
	&stats->steal);
	
	free(buf);
	
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



double get_workload_cpu(void)
{
	int busy_buf[32];
	int cnt = 0;
	int total = 0;
	
	char *line = read_path(GLOBAL_STATE, O_RDONLY);
	
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
		
		char *buf = read_path(paths[i], O_RDONLY);
		
		if (!buf)
			continue;
	
		char *endptr;
	
		long val = strtol(buf, &endptr, 10);
	
		if (endptr == buf){
			free(buf);
			continue;
		}
		
		free(buf);
	
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



void update_battery_info(int fd, bat_status_t *bs)
{
    if (fd < 0 || !bs){
    	errno = EINVAL;
    	return;
    }
    
    char *buf;
    int capacity;
    long long counter;
    char *status;

    /*
     * --------------------
     * Читаем status
     * --------------------
     */
    
    status = read_path(BAT_STATUS, O_RDONLY);
    
    if (!status){
    	print_fd(fd, "Ошибка чтения status батареи");
    	return;
    }

    /* Убираем перевод строки */
    status[strcspn(status, "\n")] = '\0';
    
    get_bat_status(bs, status);


    /*
     * --------------------
     * Читаем capacity
     * --------------------
     */

    buf = read_path(BAT_CAPACITY, O_RDONLY);
    
    if (!buf){
    	free(status);
    	print_fd(fd, "Ошибка чтения capacity");
    	return;
    }

    if (sscanf(buf, "%d", &capacity) != 1) {
        free(status);
        free(buf);
        print_fd(fd, "Ошибка разбора capacity");
        return;
    }
    
    free(buf);


    /*
     * --------------------
     * Читаем charge_counter
     * --------------------
     */
    
    buf = read_path(BAT_CHARGE_CNT, O_RDONLY);
    
    if (!buf){
    	free(status);
    	print_fd(fd, "Ошибка чтения charge_counter");
    	return;
    }

    if (sscanf(buf, "%lld", &counter) != 1) {
        free(status);
        free(buf);
        print_fd(fd, "Ошибка разбора charge_counter");
        return;
    }
    
    free(buf);


    /*
     * --------------------
     * Пишем текущее состояние
     * --------------------
     */

    char logbuf[256];

    snprintf(
        logbuf,
        sizeof(logbuf),
        "Батарея = %d%%, остаток = %lld mAh, состояние = %s\n",
        capacity,
        counter / 1000,
        status
    );

    print_fd(fd, logbuf);


    /*
     * --------------------
     * Если батарея заряжается — текущий участок разрядки закрываем.
     *
     * Накопленная статистика НЕ сбрасывается.
     * --------------------
     */

    if (bs->state != BAT_DISCHARG) {
        bs->last_capacity = -1;
        bs->last_counter = -1;
        free(status);

        return;
    }


    /*
     * --------------------
     * Начало нового участка разрядки
     * --------------------
     */

    if (bs->last_capacity < 0 ||
        bs->last_counter < 0) {

        bs->last_capacity = capacity;
        bs->last_counter = counter;
        free(status);

        return;
    }


    /*
     * --------------------
     * Считаем изменение относительно предыдущего измерения
     * --------------------
     */

    int delta_capacity = bs->last_capacity - capacity;
    long long delta_counter = bs->last_counter - counter;


    /*
     * Если fuel gauge внезапно увеличил значение,
     * не считаем это разрядкой.
     *
     * Текущую точку принимаем как новую исходную.
     */

    if (delta_capacity < 0 || delta_counter < 0) {
        bs->last_capacity = capacity;
        bs->last_counter = counter;
        free(status);

        return;
    }


    /*
     * Если процент изменился, можем использовать этот
     * интервал для оценки полной емкости.
     */

    if (delta_capacity > 0 && delta_counter > 0) {

        bs->total_capacity_delta += delta_capacity;
        bs->total_counter_delta += delta_counter;

        bs->last_capacity = capacity;
        bs->last_counter = counter;


        /*
         * ------------------
         * Достаточно накопленных данных?
         *
         * Пока ждем минимум 30 процентных пунктов разрядки.
         * ------------------
         */

        if (bs->total_capacity_delta >= 30) {

            bs->estimated_capacity =
                ((double) bs->total_counter_delta / 1000.0)
                * 100.0
                / (double) bs->total_capacity_delta;

            const double design_capacity = 5000.0;

            bs->soh =
                bs->estimated_capacity
                / design_capacity
                * 100.0;


            /*
             * ----------------
             * Записываем результат.
             * Накопленные данные НЕ сбрасываем.
             * ----------------
             */

            snprintf(
                logbuf,
                sizeof(logbuf),
                "Накопленная разрядка = %d%%, израсходовано = %lld mAh\n"
                "Оценка полной емкости АКБ = %.0f mAh\n"
                "Оценка состояния АКБ = %.1f%%\n",
                bs->total_capacity_delta,
                bs->total_counter_delta / 1000,
                bs->estimated_capacity,
                bs->soh
            );

            print_fd(fd, logbuf);
        }
    }
    free(status);
}