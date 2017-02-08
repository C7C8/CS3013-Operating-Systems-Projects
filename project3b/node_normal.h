#pragma once
#include <pthread.h>

node* initNormalNode(node* this, int newPosx, int newPosy);
void normalRecieve(node* this, message msg); //This is c++, I get to to use 'this' here without problem
void normalNodeMain(node* this); 
