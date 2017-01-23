#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>


void execCMD(char* args[3]);

int main(){
	//Design: there should be a list of "command"
	//structs that have an arguments array. This array shall be
	//expandable and shall start initialized with the three default
	//commands (whoami, last, ls). The program shall not make a
	//distinction between "built in" commands and other commands -
	//additional arguments should always be possible in all
	//situations; there should be a parsing function dedicated to
	//this that simply scans for spaces and separates arguments
	//using them.

	//An array of arrays of arguments
	char** cmdlist[10] = {	{ "whoami", NULL},
				{ "last", NULL},
				{ "ls", "--color=tty", NULL},
				{ NULL },
				{ NULL },
				{ NULL }, //ugh, at least it works
				{ NULL },
				{ NULL },
				{ NULL },
				{ NULL }};
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
		if (option >= 48 && option <= 57 && option - 48 < cmdcount)
			execCMD(cmdlist[option - 48]);

		switch (option) {
			case 'a' :
				printf("It seems you've stumbled on an unimplemented feature, Commander!\n");
				continue;
			case 'c' :
				printf("It seems you've stumbled on an unimplemented feature, Commander!\n");
				continue;
			case 'e' :
				exit(0);
			case 'p' :
				/* I swear, there's a reeaaaal good reason for the nop statement below. GCC
				 * as usual whined and complained in the most unusual way, but this time it
				 * objectd to having the declaration for cwd be right below the case statement
				 * -- something about "a label can only be part of a statement and a decclaration
				 *  is not a statement." I don't know what that's about, but I DO know that putting
				 *  literally anything that isn't a declaration here makes it go away, so...
				 *  2+2=4. It's going to get optimized out by the compiler anyways and I'm
				 *  too lazy to figure out a better solution. Don't think too poorly of me
				 *  for this... */
				2 + 2 == 4;
				char* cwd = getcwd(NULL, 0);
				printf("Current working directory: %s\n", cwd);
				free(cwd);
				continue;
		}

		printf("Commander, that isn't a command I can execute... are you okay?\n");
	}
	return 0;
}

void execCMD(char** args){
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
		printf("Page faults (reclaimed): %d\n\n", (int)usage.ru_minflt);
	}
}
