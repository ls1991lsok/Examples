#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	int result = 0;

	if (argc != 4){
		printf("Please follow the format as: ./test_calc param1 operation param2\n");
		result = -1;
		long print_result = syscall(400,0,0,'+',&result);
		return -1;
	}else{
		int len_pr1 = strlen(argv[1]);
		int len_pr2 = strlen(argv[3]);
		int i;
		int param1 = atoi(argv[1]);
		int param2 = atoi(argv[3]);

		for (i = 0; i<len_pr1; i++){
			if(!isdigit(argv[1][i])){
				printf("NaN\n");
				result = -1;
				long print_result = syscall(400,param1,param2,*argv[2],&result);
				return -1;
			}
		}

		for (i = 0; i<len_pr2; i++){
			if(!isdigit(argv[3][i])){
				printf("NaN\n");
				result = -1;
				long print_result = syscall(400,param1,param2,*argv[2],&result);
				return -1;
			}
		}

		if (*argv[2] != '+' && *argv[2] != '-' && *argv[2] != '*' && *argv[2] != '/'){
			printf("NaN\n");
			result = -1;
			long print_result = syscall(400,param1,param2,*argv[2],&result);
			return -1;
		}

		long print_result = syscall(400,param1,param2,*argv[2],&result);
		//printf("argv1 = %d\n", param1);
		//printf("argv2 = %s\n", argv[2]);
		//printf("argv3 = %d\n", param2);
		//perror("");
		//printf("The syscall return result: %ld\n", print_result);
		//printf("result value: %d\n", result);
		//printf("result address: %p\n", &result);
		if (print_result == -1){
			printf("NaN\n");
			return -1;
		}else{
			printf("%d\n",result);
		}
		return 0;
	}
}
