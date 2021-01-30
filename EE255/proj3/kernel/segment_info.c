#include <linux/mm_types.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/compat.h>

SYSCALL_DEFINE1(show_segment_info, int, pid){
	pid_t task_pid;

	if(pid == 0){
		task_pid = current->pid;
	}else{
		task_pid = pid;
	}

	struct task_struct *mytask;
	struct mm_struct *mymm;

	mytask = find_task_by_pid_ns(task_pid, &init_pid_ns);
	if(mytask){
		mymm = mytask->active_mm;
		printk("[Memory segment addresses of process %d]\n", mytask->pid);
		printk("%lx - %lx: code segment\n", mymm->start_code, mymm->end_code);
		printk("%lx - %lx: data segment\n", mymm->start_data, mymm->end_data);
		printk("%lx - %lx: heap segment\n", mymm->start_brk, mymm->brk);
		return 0;
	}else{
		printk("No Such Pid Task!");
		return -1;
	}

	return -1;
}

