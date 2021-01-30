#define _GUN_SOURCE
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/compat.h>
#include <linux/proc_ns.h>
#include <linux/proc_fs.h>
#include <linux/sched/task.h>


SYSCALL_DEFINE3(set_rtmon, pid_t, pid, struct timespec*, C, struct timespec*, T){
	if(((C->tv_sec * 1000000000 + C->tv_nsec) > (T->tv_sec * 1000000000 + T->tv_nsec)) ||(C->tv_sec == 0 && C->tv_nsec == 0) || (T->tv_sec == 0 && T->tv_nsec == 0) || C->tv_sec > 10 || T->tv_sec > 10 || (C->tv_sec == 0 && C->tv_nsec < 1000000) || (T->tv_sec == 0 && T->tv_nsec < 1000000)){
		printk("Enter parameters Wrong!");
		return -1;
	}

	pid_t task_pid;
	if(pid == 0){
		task_pid = current->pid;
	}else if(pid > 0){
		task_pid = pid;
	}else{
		printk("No such pid!");
		return -1;
	}
	

	struct task_struct *mytask;
	mytask = find_task_by_pid_ns(task_pid, &init_pid_ns);
	if(mytask){
		if((mytask->C.tv_sec + mytask->C.tv_nsec == 0) && (mytask->T.tv_sec + mytask->T.tv_nsec == 0)){
			mytask->C.tv_sec = C->tv_sec;
			mytask->C.tv_nsec = C->tv_nsec;
			mytask->T.tv_sec = T->tv_sec;
			mytask->T.tv_nsec = T->tv_nsec;
			return 0;
		}else{
			printk("Clear C & T before you set them up!");
			return -1;
		}
	}else{
		printk("Wrong task!");
		return -1;
	}
}


