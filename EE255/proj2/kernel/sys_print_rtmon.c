#define _GUN_SOURCE
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/compat.h>
#include <linux/proc_ns.h>
#include <linux/proc_fs.h>
#include <linux/sched/task.h>

SYSCALL_DEFINE1(print_rtmon, pid_t, pid){
	pid_t task_pid;
	if(pid >= 0){
		
		if(pid == 0){
			task_pid = current->pid;
		}else{
			task_pid = pid;
		}
		
		struct task_struct *mytask;
		mytask = find_task_by_pid_ns(task_pid, &init_pid_ns);
		if(mytask){
			unsigned long long print_C = mytask->C.tv_sec*1000 + mytask->C.tv_nsec/1000000;
			unsigned long long print_T = mytask->T.tv_sec*1000 + mytask->T.tv_nsec/1000000;
			printk("print_rtmon: PID %d, C %lld ms, T %lld ms", mytask->pid, print_C, print_T);
			return 0;
		}else{
			printk("No such pid task!");
			return -1;
		}
	}else if(pid == -1){
		
		struct task_struct *process, *thread;
		rcu_read_lock();
		for_each_process_thread(process, thread){
			unsigned long long print_C = thread->C.tv_sec + thread->C.tv_nsec;
			unsigned long long print_T = thread->T.tv_sec + thread->T.tv_nsec;
			if(print_C != 0 && print_T != 0){
				print_C = thread->C.tv_sec*1000 + thread->C.tv_nsec/1000000;
				print_T = thread->T.tv_sec*1000 + thread->T.tv_nsec/1000000;
				printk("print_rtmon: PID %d, C %lld ms, T %lld ms", thread->pid, print_C, print_T);
			}
		}
		rcu_read_unlock();
		return 0;		
	}else{
		printk("No Valid Pid!");
		return -1;
	}
	return -1;
}
