#include "node_normal.h"
#include <string.h>
#include <stdlib.h>

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
	this->channel = 0;
	gettimeofday(&this->lastDwell, NULL);

	//Function stuff
	this->recieve = &normalRecieve;
	this->nodeMain = &normalNodeMain;

	//Node log stuff
	char fname[7];
	snprintf(fname, 7, "%d.log", this->nodeID);
	this->log = fopen(fname, "a");
	fprintf(this->log, "====NODE %d LOG BEGIN====\n", this->nodeID);
}

void normalRecieve(node* this, unsigned int msg, unsigned int channel) {
	if (this->channel != channel){
		printf("Node %d discarded a received message -- it's listening on channel %d but the message arrived on channel %d\n!",
			   this->nodeID, this->channel, channel);
		return;
	}

	pthread_mutex_lock(&this->msgQueueLock);
	if (!getMessage(this->processedHead, msg) && !getMessage(this->msgQueueHead, msg))
	{
		addMessage(this->msgQueueHead, msg);
		printf(/*this->log,*/ "Node %d received message ID:%d\n", this->nodeID, msg);
	}
	pthread_mutex_unlock(&this->msgQueueLock);
}

void* normalNodeMain(void* val){
	node* this = (node*) val;
	while (1){
		usleep(TALK_WINDOW_TIME); //5 msTime delay so the node isn't constantly trying to send messages
		printf("Node %d has woken up!\n", this->nodeID);

		//Are we going to change channels?
		struct timeval ctime, delta;
		gettimeofday(&ctime, NULL);
		timersub(&ctime, &this->lastDwell, &delta);
		if ((float)delta.tv_usec / 1000000.f > DWELL_DURATION){
			printf("Node %d considering whether to change channels\n", this->nodeID);
			gettimeofday(&this->lastDwell, NULL);
			if (rand() % 101 > DWELL_PROBABILITY) {
				this->channel = (unsigned int) (rand() % 3);
				printf("Node %d switching to channel %d\n", this->nodeID, this->channel);
			}
		}

		//Are we going to send a message?
		if ((rand() % 101 <= TALK_PROBABILITY)){
			//Append a message to our message queue
			unsigned int msg = 0;

			pthread_mutex_lock(&msgCountMutex);
			printf("Node %d wants to broadcast message ID:%d soon\n", this->nodeID, msgCount);
			msg = msgCount++;
			pthread_mutex_unlock(&msgCountMutex);

			pthread_mutex_lock(&this->msgQueueLock);
			addMessage(this->msgQueueHead, msg);
			pthread_mutex_unlock(&this->msgQueueLock);
		}


		if (getMessage(this->msgQueueHead, 0)){
			printf("Node %d wants to transmit messages in its queue\n", this->nodeID);

			//Try to broadcast to the nearby nodes. First, though, we need to acquire relevant broadcast locks
			if (pthread_mutex_trylock(&this->broadcastLock)) {
				printf("Node %d can't broadcast now, it's receiving from elsewhere\n", this->nodeID);
				continue; //from top of node loop
			}

			int failure = 0;
			int locked[NUM_NODES] = { 0 };
			for (int i = 0; i < this->neighborCount; i++) {
				if (this->neighbors[i]->channel != this->channel)
					continue;
				if (pthread_mutex_trylock(&this->neighbors[i]->broadcastLock)) {
					printf("Node %d failed to acquire lock on node %d, unlocking nodes\n", this->nodeID, this->neighbors[i]->nodeID);
					failure = 1;
					break;
				}
				printf("Node %d is LOCKED by node %d\n", this->neighbors[i]->nodeID, this->nodeID);
				locked[i] = 1;
			}

			//If broadcast lock could not be acquired, roll back changes
			if (failure) {
				for (int i = 0; i < this->neighborCount; i++)
				{
					if (locked[i]){
						printf("Node %d is UNLOCKED by node %d after failure to acquire lock\n",
							   this->neighbors[i]->nodeID, this->nodeID);
						pthread_mutex_unlock(&this->neighbors[i]->broadcastLock);
					}
				}
				pthread_mutex_unlock(&this->broadcastLock);
				continue; //from top of node loop
			}

			//Now that we've locked ourself and all nearby nodes, actually broadcast things! In this case, just go
			//through the whole message queue and broadcast messages that need broadcasting.
			while (getMessage(this->msgQueueHead, 0)){
				unsigned int msg = getMessage(this->msgQueueHead, 0);
				printf("Node %d broadcasting message %d\n", this->nodeID, msg);
				for (int j = 0; j < this->neighborCount; j++)
					if (locked[j])
						this->neighbors[j]->recieve(this->neighbors[j], msg, this->channel);

				pthread_mutex_lock(&this->msgQueueLock);
				delMessage(this->msgQueueHead, 0);
				addMessage(this->processedHead, msg);
				pthread_mutex_unlock(&this->msgQueueLock);
			}

			//Broadcast over, folks, let's go home... I mean, unlock our neighbors. Wait, is that a better or worse statement?
			for (int i = 0; i < this->neighborCount; i++) {
				if (locked[i]) {
					pthread_mutex_unlock(&this->neighbors[i]->broadcastLock);
					printf("Node %d is UNLOCKED by node %d\n", this->neighbors[i]->nodeID, this->nodeID);
				}
			}
			pthread_mutex_unlock(&this->broadcastLock);
		}
		pthread_mutex_unlock(&this->msgQueueLock);

		printf("Node %d going to sleep for %f ms\n", this->nodeID, TALK_WINDOW_TIME / 1000.f);
	}
}

	
