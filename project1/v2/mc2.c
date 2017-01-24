#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h> //let's do some multithreading

#define PID_MAX 32768
#define ARG_MAX	32

typedef struct linkedCmd linkedCmd;
struct linkedCmd {
	char* args[ARG_MAX];
	linkedCmd* next;
	int concurrent;
};

typedef struct process process;
struct process {
	char** args;
	int pid;
	struct timeval startTime;
};

void execCmd(linkedCmd* cmd, process** ptable);
void addCmd(linkedCmd* root, linkedCmd* newCmd);
linkedCmd* getCmd(linkedCmd* root, int pos);
void printStats(struct rusage usage, struct timeval startTime);

//Process monitor thread - prefixed with td_
void* td_processMonitor(void* ptable);

int main(){
	//COMMAND LIST STUFF
	linkedCmd* cmdList = (linkedCmd*)malloc(sizeof(linkedCmd));
	addCmd(cmdList, (linkedCmd*)malloc(sizeof(linkedCmd)));
	addCmd(cmdList, (linkedCmd*)malloc(sizeof(linkedCmd)));

	//Have to manually assign these values using strcpy. There might be a better way, but
	//right now I'm feeling too lazy to do anything about it.
	getCmd(cmdList, 0)->args[0] = strdup("whoami");
	getCmd(cmdList, 1)->args[0] = strdup("last");
	getCmd(cmdList, 2)->args[0] = strdup("ls"); getCmd(cmdList, 2)->args[1] = strdup("--color=tty");
	int cmdcount = 3;

	//PROCESS TABLE
	//Why this particular PID limit? Run `sysctl kernel.pid_max`, it'll tell you that the maximum
	//number processes that the kernel allows is 32,768. You can't blame me for not supporting
	//an unlimited amount of processes when the kernel itself doesn't allow it!
	process* ptable[PID_MAX] = { NULL };

	//Misc things
	unsigned long inputSize = 128;
	char* input = (char*)malloc(inputSize);
	pthread_t reaperThread;
	pthread_create(&reaperThread, NULL, td_processMonitor, ptable);


	while (1) {
		printf("\n===== Mid-Day Commander, v2 =====\n"); //otherwise known as fake-bash...
		printf("G'day, Commander! What command would you like to run?\n");
		
		for (int i = 0; i < cmdcount; i++){
			char** args = getCmd(cmdList, i)->args;
			printf("\t%d. ", i);
			for (int j = 0; args[j] != NULL; j++)
				printf("%s ", args[j]);
			printf("%s\n", getCmd(cmdList, i)->concurrent ? "&" : " ");
		}

		printf("\ta. add command : Adds a new commmand to the menu.\n");
		printf("\tc. change directory : Changes process working directory\n");
		printf("\te. exit : Leave Mid-Day Commander\n");
		printf("\tp. pwd : Prints working directory\n");
		printf("\tr. running processes : Print a list of running processes\n");
		printf("Option?: ");
		if (getline(&input, &inputSize, stdin) == -1)
			input[0] = 'e'; //Exit at end-of-file by cheating
		printf("\n");

		if (input[0] >= 48 && input[0] <= 57){ //cheat at number detection
			int option = atoi(input);
			if (option >= 0 && option < cmdcount)
				execCmd(getCmd(cmdList, option), ptable);
			else
				printf("Commander, I don't have that many commands...\n"); //TODO: implement verbal abusiveness
			continue;
		}

		switch (input[0]) {
			case 'a' : {
				printf("-- Add a command --\n");
				printf("What command would you like to add, Commander?: ");
				linkedCmd* newCmd = (linkedCmd*)malloc(sizeof(newCmd));
				unsigned long size = 1024;
				char* str = (char*)malloc(size);
				if (getline(&str, &size, stdin) == -1){
					fprintf(stderr, "Recieved EOF at unexpected time\n");
					exit(1);
				}

				str[strlen(str)-1] = '\0'; //chop off the newline
				newCmd->args[0] = strtok(str, " ");
				newCmd->concurrent = 0;

				for (int i = 1; i < ARG_MAX; i++){
					newCmd->args[i] = strtok(NULL, " ");
					if (newCmd->args[i] == NULL)
						break;
					if (newCmd->args[i][0] == '&'){
						newCmd->concurrent = 1;
						newCmd->args[i] = NULL; //leaky?
						break;
					}
									}

				addCmd(cmdList, newCmd);
				cmdcount++;
				continue;
			}
			case 'c' : {
				printf("-- Change Directory --\n");
				printf("What directory would you like to change to, Commander?: ");
				unsigned long size = 1024;
				char* newWD = (char*)malloc(size);
				if (getline(&newWD, &size, stdin) == -1){
					fprintf(stderr, "Recieved EOF at unexpected time\n");
					exit(1);
				}

				newWD[strlen(newWD)-1] = '\0'; //chop off the newline
				if (chdir((const char*)newWD) != 0)
					printf("I encountered an error when trying to switch to that directory, Commander, but I don't care enough to tell you what it was!\n");
				free(newWD);
				continue;
			}
			case 'e' : {
				//Check for children still alive
				int canQuit = 1;
				for (int i = 0; i < PID_MAX; i++){
					if (ptable[i] != NULL){
						printf("I'm sorry Dave... err, Commander, I'm afraid I can't do that. There are still processes in my process table that need to die.\n");
						canQuit = 0;
						break;
					}
				}

				if (!canQuit)
					continue;
				printf("Goodbye, Commander...\n");
				printf("--crmyers 2017\n");
				pthread_cancel(reaperThread);
				exit(0);
				//Where'd all the memory for cmdList go? Well, it's only a memory leak if you
				//lose access to it while the program runs... right?
			}
			case 'p' : { //Braces needed because we're declaring a new variable on the first line
				char* cwd = getcwd(NULL, 0);
				printf("-- Current Directory --\n");
				printf("Directory: %s\n", cwd);
				free(cwd);
				continue;
			}
			case 'r' :
				printf("-- Background Processes --\n");
				for (int i = 0; ptable[i] != NULL; i++){
					printf("[%d]. %d: ", i, ptable[i]->pid);
					for (int j = 0; ptable[i]->args[j] != NULL; j++)
						printf("%s ", ptable[i]->args[j]);
					printf("\n");
				}
				continue;
		}

		printf("Commander, that isn't a command I can execute... are you okay?\n");
		fflush(stdin); //wait, what?
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

void execCmd(linkedCmd* cmd, process** ptable){
	struct timeval startTime;
	gettimeofday(&startTime, NULL);

	int pid = fork();
	if (pid == 0)
		execvp(cmd->args[0], (char** const)cmd->args);
	else if (cmd->concurrent){
		//Put this background process on the ptable in the first available slot
		process* newProcess = (process*)malloc(sizeof(process));
		newProcess->args = (char**)cmd->args;
		newProcess->pid = pid;
		gettimeofday(&(newProcess->startTime), NULL);
		for (int i = 0; i < PID_MAX; i++){
			if (ptable[i] == NULL){
				ptable[i] = newProcess;
				printf("Assigning background process #%d slot %d on the process table.\n", pid, i);
				break;
			}
		}
	}
	else {
		//Run this command immediately and wait until it's complete
		struct rusage usage;
		wait4(pid, 0, 0, &usage);
		printStats(usage, startTime);
	}
}

void* td_processMonitor(void* data){
	process** ptable = (process**)data;

	//Loop over everything in the ptable looking for dead children.
	//Reap them and print their stats
	struct rusage usage;
	int i = PID_MAX;
	while (1){
		i++;
		if (i >= PID_MAX)
			i = 0;

		/* ♫ Don't fear the reaper... ♫ */
		if (ptable[i] != NULL && wait4(ptable[i]->pid, 0, WNOHANG, &usage) != 0){
			struct timeval currentTime;
			gettimeofday(&currentTime, NULL);
			
			printf("----Process \"");
			for (int j = 0; ptable[i]->args[j] != NULL && j < ARG_MAX; j++)
				printf("%s ", ptable[i]->args[j]);
			printf("\" has completed ----");
			printStats(usage, ptable[i]->startTime);

			free(ptable[i]);
			ptable[i] = NULL;
		}
	}

}

void printStats(struct rusage usage, struct timeval startTime){
	//No need to subtract rusage stats, wait4 doesn't seem to return cumulative stats...
	printf("\n-- Statistics --\n");
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	float timeDelta = (float)(currentTime.tv_usec - startTime.tv_usec) / 1000000.0 + (float)(currentTime.tv_sec - startTime.tv_sec);
	printf("Elapsed time: %f s\n", timeDelta);
	printf("Page faults: %d\n", (int)usage.ru_majflt);
	printf("Page faults (reclaimed): %d\n\n", (int)usage.ru_minflt);
}
