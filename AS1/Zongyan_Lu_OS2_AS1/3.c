#include <stdio.h>
#include <string.h>
#include <sys/types.h>

int main(){
	FILE *fp;
	int i = 0;

	fp = fopen("JUNK.txt", "w");
	if( fp == NULL){
		printf("JUNK.txt failed to open!\n");
		return 1;
	}

	fprintf(fp, "%s", "Before fork\n");
	int pid = fork();

	// child process
	if(pid == 0){
		for(i = 0; i < 10; i++){
			fprintf(fp, "%s", "child\n");
		}//end for
	}//end if
	//parent process
	else{
		for(i = 0; i < 10; i++){
			fprintf(fp, "%s", "parent\n");
		}//end for
	}//end else


	//The output after the prog running: ten times "parent" always print out first, then ten times "child" come follow.

	fclose(fp);

	return 0;
}
