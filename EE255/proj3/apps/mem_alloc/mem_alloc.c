
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>


#define BILLION 1000000000L


int main(int argc, char**argv)
{
	if(argc == 1){
		printf("Please enter the memory allocation size in bytes!");
		return -1;
	}else if(argc == 2){
		uint32_t memBytes = atol(argv[1]);
		uint32_t totalTime = 0;
		uint32_t i = 0;

		char *buf = (uint8_t*) malloc(memBytes);

		struct timespec startTime, endTime;

		clock_gettime(CLOCK_MONOTONIC, &startTime);

		for(i = 0; i < memBytes; i += 4096){
    			buf[i] = 1;
		}

		clock_gettime(CLOCK_MONOTONIC, &endTime);

		totalTime = BILLION * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_nsec - startTime.tv_nsec);

		printf("Total memory access time: %u nsec\n", totalTime);

		/*This statement is just used for testing. Please do NOT deduct pts. Feel free to delete it*/
		printf("PID: %d\n", getpid());	

		int test_info;

		test_info = syscall(405, getpid());

		test_info = syscall(406, getpid());

		free(buf);

		pause();

  		return 0;
	}else{
		printf("Wrong format! Only one input parameter!");
		return -1;
	}
}
