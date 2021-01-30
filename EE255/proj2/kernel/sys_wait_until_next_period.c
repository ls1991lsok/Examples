#define _GUN_SOURCE
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/compat.h>
#include <linux/proc_ns.h>
#include <linux/proc_fs.h>
#include <linux/sched/task.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

SYSCALL_DEFINE0(wait_until_next_period){
	if(((current->C.tv_sec + current->C.tv_nsec) != 0) && ((current->T.tv_sec + current->T.tv_nsec != 0))){
		//struct hrtimer hr_timer;
		ktime_t time_wait = (current->T.tv_sec * 1000000000 + current->T.tv_nsec) - (current->C.tv_sec * 1000000000 + current->C.tv_nsec);
		//hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL_PINNED);
		set_current_state(TASK_INTERRUPTIBLE);
		//hrtimer_start(&hr_timer, time_wait, HRTIMER_MODE_REL);
		int next_start;
		next_start = schedule_hrtimeout_range(&time_wait, 0, HRTIMER_MODE_REL);
		if (next_start == 0){
			printk("hrtimer expires!");
			return 0;
		}else{
			printk("Wake up!");
			return 0;
		}
	}else{
		printk("Something bad Happened!");
		return -1;
	}
}
