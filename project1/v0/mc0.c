#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

void execCMD(char* args[3]);

int main(){
	
	while (1) {
		printf("\n===== Mid-Day Commander, v0 =====\n"); //otherwise known as fake-bash...
		printf("G'day, Commander! What command would you like to run?\n");
		printf("\t0. whoami\t: Prints out the result of the whoamicommand\n");
		printf("\t1. last\t: Prints out the result of the last command\n");
		printf("\t2. ls\t: Prints out the result of a listing on a user-specified path\n");
		printf("Option?: ");
		int option = getchar() - 48; //magic number to convert ASCII number to actual number
		getchar(); //Skip the newline. This is hacky, but it works. Just don't do CTRL-D.
		printf("\n");

		char* args[4] = {"", NULL, NULL, NULL};
		if (option == 2){ //Special processing for ls
			unsigned long len = 0;
			printf("Arguments?: "); //Get arguments...
			getline(&args[1], &len, stdin);
			args[1][strcspn(args[1], "\n")] = '\0';

			printf("Path?: ");
			getline(&args[2], &len, stdin);
			args[2][strcspn(args[2], "\n")] = '\0';
		}

		switch (option){
			case 0:
				args[0] = "whoami";
				break;
			case 1:
				args[0] = "last";
				break;
			case 2:
				args[0] = "ls";
				break;
			default:
				//Problem exists between chair and keyboard
				printf("Invalid command!\n"); 
		}
		execCMD(args);
	}
	return 0;
}

void execCMD(char* args[3]){
	struct timeval ctime;
	gettimeofday(&ctime, NULL);

	int pid = fork();
	if (pid == 0){
		execvp(args[0], args);
	}
	else {
		struct rusage usage;
		wait4(pid, 0, 0, &usage);

		struct timeval ntime;
		gettimeofday(&ntime, NULL);
		float timeDelta = (float)(ntime.tv_usec - ctime.tv_usec) / 1000.0;
		printf("Elapsed time: %f ms\n", timeDelta);
		printf("\nPage faults: %d\n", (int)usage.ru_majflt);
		printf("Page faults (reclaimed): %d\n", (int)usage.ru_minflt);
	}
}
