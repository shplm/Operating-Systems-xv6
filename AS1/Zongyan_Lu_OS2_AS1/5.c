#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){

	printf("------------------------\n");
	printf("Prob 5:\n");
	printf("------------------------\n");
	int pid;
	int i;

	for(i = 0; i < 4; i++){
		pid = fork();
		if(i == 0){
			if(0 == pid){
				printf("execl()\n");
				int status = execl(
				"/bin/ls"
				, "ls", "-l", "-F", "-h", (char *) NULL);
			}
			else{
				int stat_loc;
				pid = wait(&stat_loc);
			}
		}//end if
		else if(i == 1){
			if(0 == pid){
				printf("execlp()\n");
				int status = execlp(
				"ls"
				, "ls", "-l", "-F", "-h", (char *) NULL);
			}
			else{
				int stat_loc;
				pid = wait(&stat_loc);
			}
		}//end else if
		else if(i == 2){
			printf("execv()\n");
			if(0 == pid){
				char * arr[] = {"ls", "-l", "-F", "-h", (char *) NULL};
				int status = execv(
				"/bin/ls"
				, arr);
			}
			else{
				int stat_loc;
				pid = wait(&stat_loc);
			}
		}//end else if
		else if(i == 3){
			printf("execvp()\n");
			if(0 == pid){
				char *ls_argv[] = {"ls", "-l", "-F", "-h", (char *) NULL};
				int status = execvp(ls_argv[0], ls_argv);
			}
			else{
				int stat_loc;
				pid = wait(&stat_loc);
			}
		}//end else if
	}//end for loop

	return 0;
}
