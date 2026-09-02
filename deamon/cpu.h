#ifndef CPU_H
#define CPU_H


typedef unsigned long long ull;

typedef struct cpu_stats_s
{
	ull user;
	ull nice;
	ull system;
	ull idle;
	ull iowait;
	ull irq;
	ull softirq;
	ull steal;
} cpu_stats_t;

// читаем информацию о CPU из stats
int read_cpu_stats(cpu_stats_t *stats);


// расчет загружености CPU в процентах
double get_cpu_usage(void);


//читаем данный из global_state и сохраняем их в line
char *read_global_state(void);


// вычисляем среднюю загруженность цпу
double get_workload_cpu(void);


// печать загруженности цпу в лог
void print_fd_workload_cpu(int fd, double load); 


// получаем температуру ЦПУ
double get_cpu_temp(void);


// печатаем температуру ЦПУ
void print_fd_temp_cpu(int fd, double temp);


// собираем информацию и выводим состояние АКБ устройства
void update_battery_info(int fd);

#endif