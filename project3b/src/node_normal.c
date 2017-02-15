#include "node_normal.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

extern unsigned int nodeCount;

void initNormalNode(node* this, int newPosX, int newPosY){
	memset(this, 0, sizeof(node));
	this->nodeID = nodeCount++;
	this->posX = newPosX;
	this->posY = newPosY;
	this->processedHead = (message*)malloc(sizeof(message));
	this->msgQueueHead = (message*)malloc(sizeof(message));
	memset(this->msgQueueHead, 0, sizeof(message)); //0 out our processed and head messages
	memset(this->processedHead, 0, sizeof(message));
	pthread_mutex_init(&this->msgQueueLock, NULL);
	pthread_mutex_init(&this->broadcastLock, NULL);
	this->type = NODE_NORMAL;

	//Function stuff
	this->recieve = &normalRecieve;
	this->nodeMain = &normalNodeMain;

	//Node log stuff
	char fname[6];
	snprintf(fname, 6, "%d.log", this->nodeID);
	this->log = fopen(fname, "a");
	fprintf(this->log, "====NODE %d LOG BEGIN====\n", this->nodeID);
}


void normalRecieve(node* this, message* newMsg) {
	//This lock should never be used in most situations, it's here just in case.
	pthread_mutex_lock(&this->msgQueueLock);
	if (!getMessage(this->processedHead, newMsg->msgID) && !getMessage(this->msgQueueHead, newMsg->msgID)){
		addMessage(this->msgQueueHead, newMsg);
		fprintf(this->log, /*this->log,*/ "Node %d received message ID:%d\n", this->nodeID, newMsg->msgID);
	}
	pthread_mutex_unlock(&this->msgQueueLock);
}

void* normalNodeMain(void* val){
	node* this = (node*) val;
	while (1){
		usleep(5000); //5 msTime delay so the node isn't constantly trying to send messages

		//Are we going to send a message?
		if ((rand() % 101 <= TALK_PROBABILITY)){
			//Append a message to our message queue
			message* msg = (message*)malloc(sizeof(message));

			pthread_mutex_lock(&msgCountMutex);
			fprintf(this->log, "Node %d will send message ID:%d soon\n", this->nodeID, msgCount);
			msg->msgID = msgCount++;
			pthread_mutex_unlock(&msgCountMutex);

			pthread_mutex_lock(&this->msgQueueLock);
			addMessage(this->msgQueueHead, msg);
			pthread_mutex_unlock(&this->msgQueueLock);
		}

		pthread_mutex_lock(&this->msgQueueLock);
		if (getMessage(this->msgQueueHead, 0)){

			//Try to broadcast to the nearby nodes. First, though, we need to acquire relevant broadcast locks
			if (pthread_mutex_trylock(&this->broadcastLock)) {
				fprintf(this->log, "Node %d can't broadcast now, it's receiving from elsewhere\n", this->nodeID);
				continue; //from top of node loop
			}

			{ //Yes, this is supposed to be here. No, it's not remnant of a control statement
				int lockedNodeCount = 0;
				int failure = 0;
				for (; lockedNodeCount < this->neighborCount; lockedNodeCount++) {
					if (pthread_mutex_trylock(&this->neighbors[lockedNodeCount]->broadcastLock)) {
						fprintf(this->log, "Node %d failed to acquire lock on node %d, unlocking %i nodes\n", this->nodeID,
							   this->neighbors[lockedNodeCount]->nodeID, lockedNodeCount);
						failure = 1;
						break;
					}
					else
						fprintf(this->log, "Node %d is LOCKED by node %d\n", this->neighbors[lockedNodeCount]->nodeID, this->nodeID);
				}
				//If broadcast lock could not be acquired, roll back changes
				if (failure) {
					for (; lockedNodeCount >= 0; lockedNodeCount--) {
						fprintf(this->log, "Node %d rolling back changes to node %d...\n", this->nodeID,
							   this->neighbors[lockedNodeCount]->nodeID);
						pthread_mutex_unlock(&this->neighbors[lockedNodeCount]->broadcastLock);
					}
					continue; //from top of node loop
				}
			}

			//Now that we've locked ourself and all nearby nodes, actually broadcast things! In this case, just go
			//through the whole message queue and broadcast messages that need broadcasting.
			while (getMessage(this->msgQueueHead, 0)){
				message* msg = getMessage(this->msgQueueHead, 0);
				fprintf(this->log, "Node %d broadcasting message %d\n", this->nodeID, msg->msgID);
				for (int j = 0; j < this->neighborCount; j++)
					this->neighbors[j]->recieve(this->neighbors[j], msg);
				delMessage(this->msgQueueHead, msg->msgID);
				addMessage(this->processedHead, msg);
			}

			//Broadcast over, folks, let's go home... I mean, unlock our neighbors. Wait, is that a better or worse statement?
			for (int i = 0; i < this->neighborCount; i++) {
				pthread_mutex_unlock(&this->neighbors[i]->broadcastLock);
				fprintf(this->log, "Node %d unlocking node %d\n", this->nodeID, this->neighbors[i]->nodeID);
			}
			pthread_mutex_unlock(&this->broadcastLock);
		}
		pthread_mutex_unlock(&this->msgQueueLock);
	}
}

	
