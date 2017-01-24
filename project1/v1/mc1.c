#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

typedef struct linkedCmd linkedCmd;
struct linkedCmd {
	char* args[10];
	linkedCmd* next;
};

void execCmd(linkedCmd* cmd);
void addCmd(linkedCmd* root, linkedCmd* newCmd);
linkedCmd* getCmd(linkedCmd* root, int pos);

int main(){
	linkedCmd* cmdList = (linkedCmd*)malloc(sizeof(linkedCmd));
	addCmd(cmdList, (linkedCmd*)malloc(sizeof(linkedCmd)));
	addCmd(cmdList, (linkedCmd*)malloc(sizeof(linkedCmd)));

	//Have to manually assign these values using strcpy. There might be a better way, but
	//right now I'm feeling too lazy to do anything about it.
	getCmd(cmdList, 0)->args[0] = strdup("whoami");
	getCmd(cmdList, 1)->args[0] = strdup("last");
	getCmd(cmdList, 2)->args[0] = strdup("ls"); getCmd(cmdList, 2)->args[1] = strdup("--color=tty");
	
	int cmdcount = 3;

	while (1) {
		printf("\n===== Mid-Day Commander, v0 =====\n"); //otherwise known as fake-bash...
		printf("G'day, Commander! What command would you like to run?\n");
		
		for (int i = 0; i < cmdcount; i++){
			char** args = getCmd(cmdList, i)->args;
			printf("\t%d. ", i);
			for (int j = 0; args[j] != NULL; j++)
				printf("%s ", args[j]);
			printf("\n");
		}

		printf("\ta. add command : Adds a new commmand to the menu.\n");
		printf("\tc. change directory : Changes process working directory\n");
		printf("\te. exit : Leave Mid-Day Commander\n");
		printf("\tp. pwd : Prints working directory\n");
		printf("Option?: ");
		char option = getchar(); //TODO: Change input method to allow for better number input
		getchar(); //Skip the newline. This is hacky, but it works. Just don't do CTRL-D.
		printf("\n");

		//Who needs atoi anyways? Check if input is a number, verify that it's a valid command
		//if it is, and then execute said command.
		if (option >= 48 && option <= 57 && option - 48 < cmdcount){
			execCmd(getCmd(cmdList, option - 48));
			continue;
		}

		switch (option) {
			case 'a' : {
				printf("What command would you like to add, Commander?: ");
				linkedCmd* newCmd = (linkedCmd*)malloc(sizeof(newCmd));
				unsigned long size = 1024;
				char* str = (char*)malloc(size);
				getline(&str, &size, stdin);
				str[strlen(str)-1] = '\0'; //chop off the newline
				newCmd->args[0] = strtok(str, " ");
				for (int i = 1; i < 10; i++){
					newCmd->args[i] = strtok(NULL, " ");
					if (newCmd->args[i] == NULL)
						break;
				}

				addCmd(cmdList, newCmd);
				cmdcount++;
				continue;
			}
			case 'c' : {
				printf("What directory would you like to change to, Commander?: ");
				unsigned long size = 1024;
				char* newWD = (char*)malloc(size);
				getline(&newWD, &size, stdin);
				newWD[strlen(newWD)-1] = '\0'; //chop off the newline
				chdir((const char*)newWD);
				free(newWD);
				continue;
			}
			case 'e' :
				printf("Goodbye, Commander...\n");
				printf("--crmyers 2017\n");
				exit(0);
				//Where'd all the memory for cmdList go? Well, it's only a memory leak if you
				//lose access to it while the program runs... right?
			case 'p' : { //Braces needed because we're declaring a new variable on the first line
				char* cwd = getcwd(NULL, 0);
				printf("Current working directory: %s\n", cwd);
				free(cwd);
				continue;
			}
		}

		printf("Commander, that isn't a command I can execute... are you okay?\n");
		fflush(stdin);
	}
	return 0;
}

linkedCmd* getCmd(linkedCmd* root, int pos){
	//Get a CMD in an arbitrary position in the list. Useful because
	//we can't recurse very easily in the main menu.
	if (root == NULL)
		return NULL; 

	if (pos == 0)
		return root;
	return getCmd(root->next, pos - 1);
}

void addCmd(linkedCmd* root, linkedCmd* newCmd){
	//Add a cmd to the end of the list. There is no deleting of cmds;
	//why would you WANT that?
	if (root->next == NULL){
		newCmd->next = NULL;
		root->next = newCmd;
		return;
	}
	addCmd(root->next, newCmd);
}

void execCmd(linkedCmd* cmd){
	struct timeval ctime;
	gettimeofday(&ctime, NULL);

	int pid = fork();
	if (pid == 0){
		execvp(cmd->args[0], (char** const)cmd->args);
	}
	else {
		struct rusage usage;
		wait4(pid, 0, 0, &usage);

		struct timeval ntime;
		gettimeofday(&ntime, NULL);
		float timeDelta = (float)(ntime.tv_usec - ctime.tv_usec) / 1000.0;
		printf("\nElapsed time: %f ms\n", timeDelta);
		printf("Page faults: %d\n", (int)usage.ru_majflt);
		printf("Page faults (reclaimed): %d\n\n", (int)usage.ru_minflt);
	}
}
