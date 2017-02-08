#include "node_normal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void initNormalNode(node* this, int newPosX, int newPosY){
	memset(this, 0, sizeof(node));
	this->nodeID = nodeCount++;
	this->posX = newPosX;
	this->posY = newPosY;
	this->processedHead = (message*)malloc(sizeof(message));
	this->msgQueueHead = (message*)malloc(sizeof(message));
	memset(this->msgQueueHead, 0, sizeof(message));
	memset(this->processedHead, 0, sizeof(message));
	pthread_mutex_init(&this->msgQueueLock, NULL);
	this->type = NODE_NORMAL;

	//Function stuff
	this->recieve = &normalRecieve;
	this->nodeMain = &normalNodeMain;
}


void normalRecieve(node* this, unsigned int msgID){
	//This lock should never be used in most situations, it's here just in case.
	printf("Adding message to node %d\n", this->nodeID);
	pthread_mutex_lock(&this->msgQueueLock);
	message newMsg;
	newMsg.msgID = msgID;
	addMessage(this->msgQueueHead, &newMsg);
	pthread_mutex_unlock(&this->msgQueueLock);
}

void* normalNodeMain(void* val){
	node* this = (node*) val;
	while (1){
		//Spin! More or less. Just check message queue and move its contents on over to the process queue
		usleep(100);

		pthread_mutex_lock(&this->msgQueueLock);
		message* msg = NULL;
		while (1){
			msg = getMessage(this->msgQueueHead, 0);
			if (msg)
				addMessage(this->processedHead, msg);
			else
				break;
		}
		pthread_mutex_unlock(&this->msgQueueLock);
	}
}

	
