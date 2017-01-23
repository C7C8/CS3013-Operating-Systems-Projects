#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	char** cmd;
} scmd;


void execCMD(scmd cmd);

int main(){
	//Would a char*** work? Absolutely, but this is way more fun!
	scmd cmdlist[10] = {	{{ "whoami", NULL}},
				{{ "last", NULL}},
				{{ "ls", "--color=tty", NULL}},
				{{ NULL }},
				{{ NULL }},
				{{ NULL }}, //ugh, at least it works
				{{ NULL }},
				{{ NULL }},
				{{ NULL }},
				{{ NULL }}};
	int cmdcount = 3;

	while (1) {
		printf("\n===== Mid-Day Commander, v0 =====\n"); //otherwise known as fake-bash...
		printf("G'day, Commander! What command would you like to run?\n");
		
		for (int i = 0; i < cmdcount; i++)
			printf("\t%d. %s\n", i, cmdlist[i]);

		printf("\ta. add command : Adds a new commmand to the menu.\n");
		printf("\tc. change directory : Changes process working directory\n");
		printf("\te. exit : Leave Mid-Day Commander\n");
		printf("\tp. pwd : Prints working directory\n");
		printf("Option?: ");
		char option = getchar(); //TODO: Change input method to allow for better number input
		getchar(); //Skip the newline. This is hacky, but it works. Just don't do CTRL-D.

		//Who needs atoi anyways? Check if input is a number, verify that it's a valid command
		//if it is, and then execute said command.
		if (option >= 48 && option <= 57 && option - 48 < cmdcount){
			printf("Executing command #%d!\n", option - 48);
			execCMD(cmdlist[option - 48]);
			continue;
		}

		switch (option) {
			case 'a' :
				printf("It seems you've stumbled on an unimplemented feature, Commander!\n");
				continue;
			case 'c' :
				printf("What directory would you like to change to, Commander?: ");
				long size = 1024;
				char* newWD = (char*)malloc(size);
				getline(&newWD, &size, stdin);
				newWD[strlen(newWD)-1] = '\0'; //chop off the newline
				printf("Changing directory to \"%s\"...\n", newWD);
				chdir((const char*)newWD);
				free(newWD);
				continue;
			case 'e' :
				exit(0);
			case 'p' : { //Braces needed because we're declaring a new variable
				char* cwd = getcwd(NULL, 0);
				printf("Current working directory: %s\n", cwd);
				free(cwd);
				continue;
			}
		}

		printf("Commander, that isn't a command I can execute... are you okay?\n");
	}
	return 0;
}

void execCMD(scmd cmd){

	//printf("Trying to execute command as follows:\n");
	//for (int i = 0; args[i] != NULL; i++)
	//	printf("%s\n", args[i]);
	struct timeval ctime;
	gettimeofday(&ctime, NULL);

	int pid = fork();
	if (pid == 0){
		execvp(cmd.cmd[0], cmd.cmd);
	}
	else {
		struct rusage usage;
		wait4(pid, 0, 0, &usage);

		struct timeval ntime;
		gettimeofday(&ntime, NULL);
		float timeDelta = (float)(ntime.tv_usec - ctime.tv_usec) / 1000.0;
		printf("Elapsed time: %f ms\n", timeDelta);
		printf("\nPage faults: %d\n", (int)usage.ru_majflt);
		printf("Page faults (reclaimed): %d\n\n", (int)usage.ru_minflt);
	}
}
