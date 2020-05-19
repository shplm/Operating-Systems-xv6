#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
	int pid = fork();

	if(0 == pid){
		printf("hello\n");
	}
	else{
		wait(NULL);
		printf("goodbye\n");
		
	}

	return 0;
}



