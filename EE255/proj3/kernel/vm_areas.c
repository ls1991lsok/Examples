#include <linux/mm_types.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/compat.h>

SYSCALL_DEFINE1(show_vm_areas, int, pid){
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
		
		struct vm_area_struct *my_mma = mymm->mmap;

		printk("[Memory-mapped areas of process %d]", mytask->pid);

		while(my_mma != NULL){
			pgd_t *pgd;
			pud_t *pud;
			pmd_t *pmd;
			pte_t *pte;
			unsigned long current_va = my_mma->vm_start;
			unsigned long our_end = my_mma->vm_end;
			unsigned long page_count = 0;
			while(current_va != our_end){
				pgd = pgd_offset(my_mma->vm_mm, current_va);
				pud = pud_offset(pgd, current_va);
				pmd = pmd_offset(pud, current_va);
				pte = pte_offset_kernel(pmd, current_va);
				if(!pte_none(*pte)){
					page_count += 1;
				}
				current_va += 4096;
			}	
			if(my_mma->vm_flags & VM_LOCKED){
				printk("%lx - %lx: %lu bytes [L], %lu pages in physical memory", my_mma->vm_start, my_mma->vm_end, (my_mma->vm_end - my_mma->vm_start), page_count);
			}else{
				printk("%lx - %lx: %lu bytes, %lu pages in physical memory", my_mma->vm_start, my_mma->vm_end, (my_mma->vm_end - my_mma->vm_start), page_count);
			}
			my_mma = my_mma->vm_next;
		}

		return 0;
	}else{
		printk("No Such Pid Task!");
		return -1;
	}

	return -1;
}
