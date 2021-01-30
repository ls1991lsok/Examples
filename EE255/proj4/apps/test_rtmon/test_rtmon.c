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

int main(int argc, char*argv[]){
	if(argc != 5){
		printf("Correct input form: ./test_rtmon PID C T CPUID\n");
		return -1;
	}else{
		unsigned int my_pid = atoi(argv[1]);
		unsigned long my_C = atoi(argv[2]);
		unsigned long my_T = atoi(argv[3]);
		unsigned int my_cpuid = atoi(argv[4]);

		if((my_C > my_T)){
			printf("C should be equal to or smaller than T!\n");
			return -1;
		}else if(my_cpuid < 0 || my_cpuid > 3){
			printf("CPUID range should be 0 to 3 for RPI!\n");
			return -1;
		}else{
			
			int check;
			int cancel_check;
			struct timespec C, T;
			
			C.tv_sec = my_C/1000;
			C.tv_nsec = (my_C - C.tv_sec * 1000) * 1000000;
			T.tv_sec = my_T/1000;
			T.tv_nsec = (my_T - T.tv_sec * 1000) * 1000000;

			check = syscall(401, my_pid, &C, &T, my_cpuid);

			if(check == 0){
				printf("Setting parameters successfully!\n");
				printf("PID: %d, C: %lu, T:%lu, CPUID: %d\n", my_pid, my_C, my_T, my_cpuid);
				return 0;
			}else{
				printf("Setting parameters failed!\n");
				return -1;
			}
		}
	}
	return 0;
}
