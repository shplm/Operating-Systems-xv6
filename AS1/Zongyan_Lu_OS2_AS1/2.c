#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
	int xx = 100;
	int pid = fork();
	printf("------------------------\n");
	printf("Prob 2\n");
	printf("------------------------\n");
	if(pid == -1){
		return -1;
	}
	else if(0 == pid){
		printf("Assign 'xx' to 100, Variable 'xx' in child process is: %d\n", xx);
		fflush(stdout);
		xx = 777;
		printf("Change 'xx' to 777, Variable 'xx' in child process is: %d\n", xx);
		fflush(stdout);
	}
	else{
		xx = 999;
		printf("Change 'xx' to 999, Variable 'xx' in parent process is: %d\n", xx);
		fflush(stdout);
	}
	printf("\n\n");
	// printf("After all changes, variable 'xx' now is: %d\n", xx);

	return 0;
}
