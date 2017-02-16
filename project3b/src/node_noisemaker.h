#pragma once
#include <pthread.h>
#include "node.h"

void initNoisyNode(node* this, int newPosx, int newPosy);
void noisyRecieve(node* this, unsigned int msg, unsigned int channel); //This is c, I get to to use 'this' here without problem
void* noisyNodeMain(void* val);
