#pragma once
#include <pthread.h>
#include "node.h"

void initNormalNode(node* this, int newPosx, int newPosy);
void normalRecieve(node* this, unsigned int msg); //This is c, I get to to use 'this' here without problem
void* normalNodeMain(void* val); 
