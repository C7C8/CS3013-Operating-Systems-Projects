#pragma once
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "params.h"

typedef struct message message;
struct message {
	unsigned int msgID;
	message* next;
};

unsigned int msgCount;
pthread_mutex_t msgCountMutex;
pthread_mutex_t startMutex; //checking the box...
pthread_cond_t startVar;

unsigned int addMessage(message* head, unsigned int msg);
unsigned int getMessage(message* head, unsigned int msgID);
unsigned int delMessage(message* head, unsigned int msgID);

typedef enum {NODE_NORMAL, NODE_NOISY} node_type;
typedef struct node node;
struct node {
	//Base node data 
	int nodeID;
	int posX;
	int posY;
	message* processedHead;
	message* msgQueueHead;
	node_type type;
	int neighborCount;
	node* neighbors[NUM_NODES];
	unsigned int channel;
	struct timeval lastDwell;

	//Synchronization stuff
	pthread_mutex_t msgQueueLock;
	pthread_mutex_t broadcastLock;

	//Message related functions
	void (*recieve)(node*, unsigned int, unsigned int);
	void* (*nodeMain)(void*);

	//Node log
	FILE *log;
};

