#include "node_normal.c"
#include <string.h>

node* initNormalNode(node* this, int newPosX, int newPosY){
	memset(this, 0, sizeof(node));
	this.nodeID = nodeCount++;
	this.posX = newPosX;
	this.posY = newPosY;
}
