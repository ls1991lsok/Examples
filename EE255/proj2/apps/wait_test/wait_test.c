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

int main(int argc, char**argv){
	int i;
	long exe;
	struct timespec C, T;
	pid_t pid;

	exe = syscall(403, pid = getpid());
	
	C.tv_sec = 0;
	C.tv_nsec = 0;
	T.tv_sec = 0;
	T.tv_sec = 0;

	exe = syscall(403, pid = getpid());

	exe = syscall(401, pid = getpid(), &C, &T);

	exe = syscall(402, pid = getpid());

	C.tv_sec = 1;
	C.tv_nsec = 5;
	T.tv_sec = 2;
	T.tv_nsec = 10;
	
	exe = syscall(401, pid = getpid(), &C, &T);

	exe = syscall(403, pid = getpid());


	while(1){
		for(i=0; i < 100000*1.2; i++);
		exe = syscall(404);
	}

	return 0;
	
	
}
