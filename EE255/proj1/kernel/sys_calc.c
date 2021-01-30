#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/linkage.h>

SYSCALL_DEFINE4(calc, int, param1, int, param2, char, operation, int*, result){
	int sum;
	int difference;
	int product;
	int quotient;
	if(*result == -1){
		printk("NaN");
		return -1;
	}
	if(operation == '+'){
		sum = param1 + param2;
		printk("The sum of %d and %d is %d\n", param1, param2, sum);
		*result = sum;
		return 0;
	}else if(operation == '-'){
		difference = param1 - param2;
		printk("The difference of %d and %d is %d\n", param1, param2, difference);
		*result = difference;
		return 0;
	}else if(operation == '*'){
		product = param1 * param2;
		printk("The product of %d and %d is %d\n", param1, param2, product);
		*result = product;
		return 0;
	}else if(operation == '/'){
		if(param2 == 0){
			printk("NaN");
			return -1;
		}else{
			quotient = param1 / param2;
			printk("The quotient of %d and %d is %d\n", param1, param2, quotient);
			*result = quotient;
			return 0;
		}
	}else{
		printk("NaN");
		return -1;
	}

}
