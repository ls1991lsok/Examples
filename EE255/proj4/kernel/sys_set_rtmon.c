#define _GUN_SOURCE
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/compat.h>
#include <linux/proc_ns.h>
#include <linux/proc_fs.h>
#include <linux/sched/task.h>


SYSCALL_DEFINE4(set_rtmon, pid_t, pid, struct timespec*, C, struct timespec*, T, int, cpuid){
	if(((C->tv_sec * 1000000000 + C->tv_nsec) > (T->tv_sec * 1000000000 + T->tv_nsec)) ||(C->tv_sec == 0 && C->tv_nsec == 0) || (T->tv_sec == 0 && T->tv_nsec == 0) || C->tv_sec > 10 || T->tv_sec > 10 || (C->tv_sec == 0 && C->tv_nsec < 1000000) || (T->tv_sec == 0 && T->tv_nsec < 1000000) || cpuid < -1 || cpuid > 3){
		printk("Enter parameters Wrong!");
		return -1;
	}
	
	if(cpuid == -1){
		unsigned int C_thread;
		unsigned int T_thread;
		unsigned int U_list[4] = {0};
		unsigned int cpu = 0;
		unsigned int maximum = 0;
		unsigned int j;
		
		struct task_struct *prc, *thd;
		for_each_process_thread(prc, thd){
			if((thd->C.tv_sec + thd->C.tv_nsec != 0) && (thd->T.tv_sec + thd->T.tv_nsec != 0)){
				cpu = thd->my_cpuid;
				C_thread = (thd->C.tv_sec*1000) + (thd->C.tv_nsec/1000000);
				T_thread = (thd->T.tv_sec*1000) + (thd->T.tv_nsec/1000000);
				U_list[cpu] += C_thread/T_thread;
				printk("PID: %d, C: %f, T: %f, CPUID: %d\n",thd->pid, C_thread, T_thread, thd->my_cpuid);
		}
		
		for(j = 0; j < 4; j++){
			if(maximum < U_list[j]){
				maximum = U_list[j];
				cpu = j;
			}
			printk("CPU %d Utilization is %f\n", j, U_list[j]);
		}

		unsigned int C_list[50];
		unsigned int T_list[50];
		unsigned int count = 0;

		struct task_struct *prcs, *thds;
		for_each_process_thread(prcs, thds){
			if(thds->my_cpuid == cpu){
				if((thds->C.tv_sec + thds->C.tv_nsec != 0) && (thds->T.tv_sec + thds->T.tv_nsec != 0)){
					C_list[count] = (thds->C.tv_sec*1000) + (thds->C.tv_nsec/1000000);
					T_list[count] = (thds->T.tv_sec*1000) + (thds->T.tv_nsec/1000000);
					count +=1;
					printk("PID: %d, C: %d, T: %d, CPUID: %d\n",thds->pid, C_list[count-1], T_list[count-1], thds->my_cpuid);
			}
		}
	}

	if(count != 0){
		C_list[count] = C->tv_sec*1000 + C->tv_nsec/1000000;
		T_list[count] = T->tv_sec*1000 + T->tv_nsec/1000000;

		unsigned int new_size = count + 1;
		unsigned int lowest_index = -1;
		unsigned int i;
		unsigned int C_low_priority = 0;
		unsigned int T_low_priority = 0;


		for(i = 0; i < new_size; i++){
			if(T_low_priority < T_list[i]){
				T_low_priority = T_list[i];
				C_low_priority = C_list[i];
				lowest_index = i;
			}else if(T_low_priority == T_list[i]){
				if(C_low_priority < C_list[i]){
					T_low_priority = T_list[i];
					C_low_priority = C_list[i];
					lowest_index = i;
				}
			}
		}

		if(lowest_index != -1){
			for(i = lowest_index; i < new_size; i++){
				T_list[i] = T_list[i + 1];
				C_list[i] = C_list[i + 1];
			}
			new_size -=1;
		}

		unsigned int R_0 = C_low_priority;
		unsigned int R_end = 0;
		unsigned int R_temp_end =0;
		unsigned int ceiling = 0;
		bool repeat_R = true;

		
		R_temp_end = R_0;
		for(i = 0; i < new_size; i++){
			if(R_0%T_list[i]){
				ceiling = (R_0/T_list[i]) + 1;
			}else{
				ceiling = (R_0/T_list[i]);
			}
			R_temp_end += ceiling * C_list[i];
		}
		
		while(repeat_R){
			R_end = R_0;
			for(i = 0; i < new_size; i++){
				if(R_temp_end%T_list[i]){
					ceiling = (R_temp_end/T_list[i]) + 1;
				}else{
					ceiling = (R_temp_end/T_list[i]);
				}
				R_end += ceiling * C_list[i];
			}
			if(R_end > T_low_priority){
				repeat_R = false;
				printk("Unschedulable!");
				return -1;
			}
			if(R_end == R_temp_end){
				repeat_R = false;
				printk("Schedulable!");
			}else{
				R_temp_end = R_end;
				repeat_R = true;
			}
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
				mytask->my_cpuid = cpuid;
			}else{
				printk("Clear C & T before you set them up!");
				return -1;
			}
		}else{
			printk("Wrong task!");
			return -1;
		}

		struct cpumask mask;
		cpumask_clear(&mask);
		cpumask_set_cpu(cpu, &mask);
		if(sched_setaffinity(pid, &mask) < 0){
			printk("Set CPU failed");
			return -1;
		}
			
	}else if(count == 0){


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
				mytask->my_cpuid = cpuid;
			}else{
				printk("Clear C & T before you set them up!");
				return -1;
			}
		}else{
			printk("Wrong task!");
			return -1;
		}

		struct cpumask mask;
		cpumask_clear(&mask);
		cpumask_set_cpu(cpu, &mask);
		if(sched_setaffinity(pid, &mask) < 0){
			printk("Set CPU failed");
			return -1;
		}

	}
	
	return 0;
		
	}	

	}else{
	unsigned int C_list[50];
	unsigned int T_list[50];
	unsigned int count = 0;

	struct task_struct *prc, *thd;
	for_each_process_thread(prc, thd){
		if(thd->my_cpuid == cpuid){
			if((thd->C.tv_sec + thd->C.tv_nsec != 0) && (thd->T.tv_sec + thd->T.tv_nsec != 0)){
				C_list[count] = (thd->C.tv_sec*1000) + (thd->C.tv_nsec/1000000);
				T_list[count] = (thd->T.tv_sec*1000) + (thd->T.tv_nsec/1000000);
				count +=1;
				printk("PID: %d, C: %d, T: %d, CPUID: %d\n",thd->pid, C_list[count-1], T_list[count-1], thd->my_cpuid);
			}
		}
	}
	
	if(count != 0){
		C_list[count] = C->tv_sec*1000 + C->tv_nsec/1000000;
		T_list[count] = T->tv_sec*1000 + T->tv_nsec/1000000;

		unsigned int new_size = count + 1;
		unsigned int lowest_index = -1;
		unsigned int i;
		unsigned int C_low_priority = 0;
		unsigned int T_low_priority = 0;


		for(i = 0; i < new_size; i++){
			if(T_low_priority < T_list[i]){
				T_low_priority = T_list[i];
				C_low_priority = C_list[i];
				lowest_index = i;
			}else if(T_low_priority == T_list[i]){
				if(C_low_priority < C_list[i]){
					T_low_priority = T_list[i];
					C_low_priority = C_list[i];
					lowest_index = i;
				}
			}
		}

		if(lowest_index != -1){
			for(i = lowest_index; i < new_size; i++){
				T_list[i] = T_list[i + 1];
				C_list[i] = C_list[i + 1];
			}
			new_size -=1;
		}

		unsigned int R_0 = C_low_priority;
		unsigned int R_end = 0;
		unsigned int R_temp_end =0;
		unsigned int ceiling = 0;
		bool repeat_R = true;

		
		R_temp_end = R_0;
		for(i = 0; i < new_size; i++){
			if(R_0%T_list[i]){
				ceiling = (R_0/T_list[i]) + 1;
			}else{
				ceiling = (R_0/T_list[i]);
			}
			R_temp_end += ceiling * C_list[i];
		}
		
		while(repeat_R){
			R_end = R_0;
			for(i = 0; i < new_size; i++){
				if(R_temp_end%T_list[i]){
					ceiling = (R_temp_end/T_list[i]) + 1;
				}else{
					ceiling = (R_temp_end/T_list[i]);
				}
				R_end += ceiling * C_list[i];
			}
			if(R_end > T_low_priority){
				repeat_R = false;
				printk("Unschedulable!");
				return -1;
			}
			if(R_end == R_temp_end){
				repeat_R = false;
				printk("Schedulable!");
			}else{
				R_temp_end = R_end;
				repeat_R = true;
			}
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
				mytask->my_cpuid = cpuid;
			}else{
				printk("Clear C & T before you set them up!");
				return -1;
			}
		}else{
			printk("Wrong task!");
			return -1;
		}

		struct cpumask mask;
		cpumask_clear(&mask);
		cpumask_set_cpu(cpuid, &mask);
		if(sched_setaffinity(pid, &mask) < 0){
			printk("Set CPU failed");
			return -1;
		}
			
	}else if(count == 0){


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
				mytask->my_cpuid = cpuid;
			}else{
				printk("Clear C & T before you set them up!");
				return -1;
			}
		}else{
			printk("Wrong task!");
			return -1;
		}

		struct cpumask mask;
		cpumask_clear(&mask);
		cpumask_set_cpu(cpuid, &mask);
		if(sched_setaffinity(pid, &mask) < 0){
			printk("Set CPU failed");
			return -1;
		}

	}
	}
	return 0;
}


