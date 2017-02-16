#include "node_noisemaker.h"
#include <string.h>
#include <stdlib.h>

extern unsigned int nodeCount;

void initNoisyNode(node* this, int newPosX, int newPosY){
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
	this->recieve = &noisyRecieve;
	this->nodeMain = &noisyNodeMain;

	//Node log stuff
	char fname[7];
	snprintf(fname, 7, "%d.log", this->nodeID);
	this->log = fopen(fname, "a");
	fprintf(this->log, "====NOISEMAKER %d LOG BEGIN====\n", this->nodeID);
}

void noisyRecieve(node* this, unsigned int msg, unsigned int channel) {
	printf("Noisy node u %d received a message... I guess the other guy didn't realize that I'm a microwave.\n", this->nodeID);
}

void* noisyNodeMain(void* val){
	pthread_cond_wait(&startVar, &startMutex);

	node* this = (node*) val;
	while (1){

		//Are we going to switch channels?
		struct timeval ctime, delta;
		gettimeofday(&ctime, NULL);
		timersub(&ctime, &this->lastDwell, &delta);
		if (delta.tv_usec > DWELL_NOISEMAKERS) {
			printf("Noisemaker node %d considering whether to change channels\n", this->nodeID);
			gettimeofday(&this->lastDwell, NULL);
			if (rand() % 101 > DWELL_PROBABILITY_NOISEMAKERS) {
				this->channel = (unsigned int) (rand() % 3);
				printf("Noisemaker node %d switching to channel %d\n", this->nodeID, this->channel);
			}
		}

		//Shall we activate?
		if (rand() % 100 < NOISEMAKER_ACTIVATE_PROBABILITY){
			printf("Noisemaker node %d at (%d, %d) activating on channel %d... *microwave BZZZZZT*\n", this->nodeID, this->posX, this->posY, this->channel);
			pthread_mutex_lock(&this->broadcastLock);
			usleep(rand() % NOISEMAKER_MAX_TIME);
			printf("Noisemaker node %d at (%d, %d) deactivating on channel %d*\n", this->nodeID, this->posX, this->posY, this->channel);
			pthread_mutex_unlock(&this->broadcastLock);
		}

		printf("Noisemaker node %d going to sleep for %f ms\n", this->nodeID, TALK_WINDOW_TIME / 1000.f);
		usleep(TALK_WINDOW_TIME);
	}
}

	
