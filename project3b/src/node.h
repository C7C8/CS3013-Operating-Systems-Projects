#pragma once
#include <pthread.h>
#include "params.h"

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
	int neighborCount;
	node* neighbors[NUM_NODES];

	//Message related functions
	void (*recieve)(node*, unsigned int);
	void* (*nodeMain)(void*); 
};
