#pragma once
#include <pthread.h>

typedef struct message message;
struct message {
	unsigned int msgID;
	message* next;
};

int addMessage(message* head, message* msg);
message* getMessage(message* head, unsigned int msgID);
int delMessage(message* head, unsigned int msgID);
	
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
	pthread_mutex_t msgQueueLock; 

	//Message related functions
	void (*recieve)(node*, unsigned int);
	void* (*nodeMain)(void*); 
};

int nodeCount = 0; //Terrible hack for getting reliable node IDs. It works for now!
