#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/linkage.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>

extern void *sys_call_table[];

static int (*original_sys_calc)(int,int,char,int*);

asmlinkage void mod_calc_func(int param1, int param2, char operation, int* result)
{
	*result = param1 % param2;
	printk("HEY! sys_exit called with %d\n", *result);
}

static int mod_calc_init(void)
{
	original_sys_calc = sys_call_table[400];
	sys_call_table[400] = mod_calc_func;
	return 0;
}

static void mod_calc_exit(void)
{
	sys_call_table[400] = original_sys_calc;
}

module_init(mod_calc_init);
module_exit(mod_calc_exit);
MODULE_LICENSE("GPL");
