#pragma once
#include <pthread.h>

typedef struct message message;
struct message {
	int msgID;
	char data[8]; //why not?
	message* next;
};
	
enum node_type {NODE_NORMAL, NODE_NOISY};
typedef struct node node;
struct node {
	//Base node data 
	int nodeID;
	int posX;
	int posY;
	message* processedHead;
	message* msgQueueHead;
	node_type type;

	//Message related functions
	void (*recieve)(node*, message);
	void (*nodeMain)(node*);

};
