#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <string.h>

void execCMD(const char* cmd, const char* args);

int main(){
	
	//Todo list:
	//1. Fix ls output
	//2. Clean up/restructure code a bit
	//3. FIND A BETTER WAY TO EXEC THINGS, THIS IS PATHETIC
	//
	//Sorry, but this is about as far as I got. It works, sort of, but I had major issues with
	//geting arguments to work correctly. They still don't entirely work right... I think I'm
	//just misunderstanding here. This will all be much more functional when the final version
	//of this thing is turned in!
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
				execCMD("/bin/ls", args);
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

	struct timeval ctime;
	gettimeofday(&ctime, NULL);

	int pid = fork();
	if (pid == 0){
		execl(cmd, args, (char*) NULL);
		exit(0);
	}
	else {
		struct rusage usage;
		wait4(pid, 0, 0, &usage);

		struct timeval ntime;
		gettimeofday(&ntime, NULL);
		float timeDelta = (float)(ntime.tv_usec - ctime.tv_usec) / 1000.0;
		printf("Elapsed time: %f ms\n", timeDelta);
		printf("\nPage faults: %d\n", usage.ru_majflt);
		printf("Page faults (reclaimed): %d\n", usage.ru_minflt);
	}
}
