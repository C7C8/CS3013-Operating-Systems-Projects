#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#include "node.h"
#include "node_normal.h"
#include "params.h"

/* Okay, my code doesn't work all the way. Not yet, anyways, but the checkpoint
 * contributions do require only a "partially working solution". TAs please
 * have mercy, I had a major team meeting with my Software Engineering team
 * alongside a boatload of work from that class (project due thursday
 * afternoon...). Anyways, since I don't have much code in good standing yet,
 * here is a plain english explanation of how I will tackle the problem:
 *
 * Briefly, nodes will each maintain a simple broadcast lock. When a node wants to broadcast, it will
 * try to acquire all of the broadcast locks to its neighbors. If it cannot do so, it will not broadcast.
 * If it does aquire all those locks, it will lock its own broadcast lock, broadcast its message, then
 * unlock everything it previously locked. Noisemakers will not send their own messages per se, when they 
 * activate they will simply lock their own lock, and will unlock it when they deactivate. This approach 
 * is very loosely adapted from my original round robin approach that was discarded due to not maximizing
 * concurrency. 
 */

int main(int argc, char* argv[]){
	srand(0); //Repeatable results, right? TODO: Initialize using time of day
	printf("Creating %d nodes (%d noisemakers)\n", NUM_NODES, NUM_NOISEMAKERS);
	node nodes[NUM_NODES];
	pthread_t threads[NUM_NODES];
	
	//TODO: Create actual noisemaker nodes
	for (int i = 0; i < NUM_NODES; i++){
		initNormalNode(&nodes[i], rand() % 100, rand() % 100);
		printf("Created node ID:%d at (%d,%d)... now starting thread!\n", nodes[i].nodeID, nodes[i].posX, nodes[i].posY);
		pthread_create(&threads[i], NULL, nodes[i].nodeMain, &nodes[i]);
	}
	return 0;
}
