#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "node_normal.h"
#include "node_noisemaker.h"


/*
 * Briefly, nodes each maintain a simple broadcast lock. When a node wants to broadcast, it will
 * try to acquire all of the broadcast locks to its neighbors. If it cannot do so, it will not broadcast.
 * If it does acquire all those locks, it will lock its own broadcast lock, broadcast its message, then
 * unlock everything it previously locked. Noisemakers will not send their own messages per se, when they 
 * activate they lock their own broadcast lock, and will unlock it when they deactivate. This approach
 * is very loosely adapted from my original round robin approach that was discarded due to not maximizing
 * concurrency. 
 */

int main(int argc, char* argv[]){
	srand(clock());
	printf("Creating %d nodes (%d noisemakers)\n", NUM_NODES, NUM_NOISEMAKERS);
	node nodes[NUM_NODES];
	pthread_t threads[NUM_NODES];
	pthread_mutex_init(&msgCountMutex, NULL);
	
	for (int i = 0; i < NUM_NODES - NUM_NOISEMAKERS; i++){
		initNormalNode(&nodes[i], rand() % 100, rand() % 100);
		printf("Created node ID:%d at (%d,%d)...\n", nodes[i].nodeID, nodes[i].posX, nodes[i].posY);
	}

	for (int i = NUM_NODES - NUM_NOISEMAKERS; i < NUM_NODES; i++){
		initNoisyNode(&nodes[i], rand() % 100, rand() % 100);
		printf("Created noisemaker ID:%d at (%d,%d)...\n", nodes[i].nodeID, nodes[i].posX, nodes[i].posY);
	}

	//Now give each node a list of its neighbors.
	for (int i = 0; i < NUM_NODES; i++){
		for (int j = 0; j < NUM_NODES; j++){
			if (i == j)
				continue;
			if (abs(nodes[i].posX - nodes[j].posX) <= 5 || abs(nodes[i].posY - nodes[j].posY) <= 5) {
				printf("Adding node:%d to neighbor list of node:%d\n", j, i);
				nodes[i].neighbors[nodes[i].neighborCount++] = &nodes[j];
			}
		}
	}

	//Start off the node threads!
	for (int i = 0; i < NUM_NODES; i ++){
		printf("Starting node %d main thread...\n", i);
		pthread_create(&threads[i], NULL, nodes[i].nodeMain, &nodes[i]);
	}

	system("sleep 10");

	//Close node files so nasty error messages don't get printed to the terminal
	for (int i = 0; i < NUM_NODES; i++)
		fclose(nodes[i].log);

	return 0;
}
