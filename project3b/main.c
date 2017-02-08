#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "node.h"
#include "node_normal.h"
#include "params.h"

int main(int argc, char* argv){
	srand(0); //Repeatable results, right? TODO: Initialize using time of day
	printf("Creating %d nodes (%d noisemakers)\n", NUM_NODES, NUM_NOISEMAKERS);
	node nodes[NUM_NODES];
	pthread_t threads[NUM_NODES];
	
	//TODO: Create actual noisemaker nodes
	for (int i = 0; i < NUM_NODES; i++){
		nodes[i] = *initNormalNode(&nodes[i], rand() % 100, rand() % 100);
		printf("Created node ID:%d at (%d,%d)... now starting thread!\n", nodes[i].nodeID, nodes[i].posX, nodes[i].posY);
		pthread_create(&threads[i], NULL, nodes[i].nodeMain, &nodes[i]);
	}
	return 0;
}
