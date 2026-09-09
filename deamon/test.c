#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "func.h"
#include "cpu.h"

int main(void)
{
	bat_status_t bs;
	int fd;
	
	if ((fd = open("/root/my_deamon/bat_condition.dat", O_RDONLY)) < 0){
			perror("open");
			return 1;
	}
	
	if (read_fd_bin(fd, &bs, sizeof(bs)) < 0){
		perror("read");
		return 1;
	}
	printf("status = %d\nlast_capacity = %d\nlast_counter = %lld\ntotal_capacity_delta = %d\ntotal_counter_delta = %lld\nestimated_capacity = %.1f\nsoh = %.1f\n", bs.state, bs.last_capacity, bs.last_counter / 1000, bs.total_capacity_delta, bs.total_counter_delta / 1000, bs.estimated_capacity, bs.soh);
	return 0;
}