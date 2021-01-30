#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/pid.h>
#include <linux/proc_ns.h>
#include <linux/proc_fs.h>
#include <linux/sched/task.h>

SYSCALL_DEFINE1(cancel_rtmon, pid_t, pid){
	pid_t task_pid;
	if(pid == 0){
		task_pid = current->pid;
	}else if(pid > 0){
		task_pid = pid;
	}else{
		printk("Error Pop!");
		return -1;
	}

	struct task_struct *mytask;
	mytask = find_task_by_pid_ns(task_pid, &init_pid_ns);
	if(mytask){
		if((mytask->C.tv_sec + mytask->C.tv_nsec == 0) && (mytask->T.tv_sec + mytask->T.tv_nsec == 0)){
			printk("Default C & T are empty!");
			return -1;
		}else{
			mytask->C.tv_sec = 0;
			mytask->C.tv_nsec = 0;
			mytask->T.tv_sec = 0;
			mytask->T.tv_nsec = 0;
		}			
	}else{
		printk("Wrong task!");
		return -1;
	}

	return 0;	
}
