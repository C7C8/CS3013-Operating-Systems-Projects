#include <unistd.h>
#include <stdio.h>
#include <string.h>

void execCMD(const char* cmd, const char* args);
//void printStats();

int main(){
	for(;;){ 
		printf("\n===== Mid-Day Commander, v0 =====\n"); //otherwise known as fake-bash...
		printf("G'day, Commander! What command would you like to run?\n");
		printf("\t0. whoami\t: Prints out the result of the whoamicommand\n");
		printf("\t1. last\t: Prints out the result of the last command\n");
		printf("\t2. ls\t: Prints out the result of a listing on a user-specified path\n");
		printf("Option?: ");
		int option = getchar() - 48; //magic number to convert ASCII number to actual number
		getchar(); //Skip the newline. This is hacky, but works. Just don't do CTRL-D.
		printf("\n");

		char* args = NULL;
		if (option == 2){ //Special processing for ls
			unsigned long len = 0;
			printf("Arguments?: "); //Get arguments...
			getline(&args, &len, stdin);
			args[strcspn(args, "\n")] = ' ';

			char* path = NULL;
			unsigned long len2 = 0;
			printf("Path?: ");
			getline(&path, &len2, stdin);
			path[strcspn(path, "\n")] = '\0';
			strcat(args, path); //Get path and add it on the arguments list
		}

		switch (option){
			case 0:
				execCMD("/usr/bin/whoami", " ");
				break;
			case 1:
				execCMD("/usr/bin/last", " ");
				break;
			case 2:
				execCMD("/usr/bin/ls", args);
				break;
			default:
				//Problem exists between chair and keyboard
				printf("Invalid command!\n"); 
		}
	}
	return 0;
}

void execCMD(const char* cmd, const char* args){
	if (cmd == NULL)
		return; //can't do anything with that...

	printf("Issuing command: %s %s", cmd, args);

	int pid = fork();
	if (pid == 0){
		execl(cmd, args, (char*) NULL);
		exit(0);
	}
	else
		wait(pid);
}
