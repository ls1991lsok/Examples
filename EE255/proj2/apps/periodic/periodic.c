#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <errno.h>
#include <sys/timerfd.h>
#include <sys/time.h>
#include <stdint.h>

//void when_to_stop(struct timespec *time, long exe_t);
//void when_to_sleep(struct timespec *time, long our_period);

int main(int argc, char**argv)
{
	unsigned int C = atoi(argv[1]);
	unsigned int T = atoi(argv[2]);
	unsigned int CPUID = atoi(argv[3]);
	if ( C>T || CPUID<0 || CPUID>3 || C > 10000 || T > 10000 || argc != 4 || C <= 0 || T <= 0){
		printf("Error Pop!");
		return -1;
	}
	printf("PID: %d, C: %d, T: %d, CPUID: %d\n", getpid(), C, T, CPUID);


	int cpu_number = sysconf(_SC_NPROCESSORS_CONF);
	cpu_set_t mask;
	cpu_set_t get;
	CPU_ZERO(&mask);
	CPU_SET(CPUID, &mask);
	if(sched_setaffinity(0, sizeof(mask), &mask) < 0){
		printf("Set CPU fail, ERROR:%s\n", strerror(errno));
	}


	struct timespec next;
	struct itimerspec ittimer;
	int fd;
	int settime;
	unsigned int i;
	unsigned int exe_t = C*100000*1.2;  //approximate the execution time
	unsigned int our_period = T;
	//clock_gettime(CLOCK_MONOTONIC, &next);
	ittimer.it_value.tv_sec = our_period/1000;
	ittimer.it_value.tv_nsec = (our_period - ittimer.it_value.tv_sec*1000)*1000000;
	ittimer.it_interval.tv_sec = our_period/1000;
	ittimer.it_interval.tv_nsec = (our_period - ittimer.it_interval.tv_sec*1000)*1000000;
	fd = timerfd_create(CLOCK_MONOTONIC, 0);
	if(fd<0){
		printf("timer_create error\n");
		return -1;
	}
	settime = timerfd_settime(fd, 0, &ittimer,NULL);
	if(settime < 0){
		printf("timer_set error\n");
		return -1;
	}
	while(1){
		for(i=0;i<exe_t;i++);
		uint64_t data;
		read(fd, &data, sizeof(uint64_t));
	}
	return 0;
}

/*void when_to_stop(struct timespec *time, long exe_t){
	time->tv_nsec += exe_t*1000000;
	if(time->tv_nsec > 10000000000){
		time->tv_nsec = time->tv_nsec - 1000000000;
		time->tv_sec += 1;
	}
}*/

/*void when_to_sleep(struct timespec *time, long our_period){
	time->tv_nsec += our_period*1000000;
	if(time->tv_nsec > 10000000000){
		time->tv_nsec = time->tv_nsec - 1000000000;
		time->tv_sec += 1;
	}
}*/
