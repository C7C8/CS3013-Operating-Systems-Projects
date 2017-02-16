#include <malloc.h>
#include "node.h"

unsigned int nodeCount = 0; //Terrible hack for getting reliable node IDs. It works for now!
unsigned int msgCount = 1;

//Adds a message to the given linked list head. Returns true if add was successful, false if add failed
unsigned int addMessage(message* head, unsigned int msg){
	//The message must be dynamically allocated and copied.
	message* mMsg = (message*)malloc(sizeof(message));
	mMsg->msgID = msg;
	mMsg->next = head->next;
	head->next = mMsg;

	return 1;
}

//Returns message of given ID using the given linked list head. If 0 is passed in for an ID, the first message will be returned.
//Returns null on failure.
unsigned int getMessage(message* head, unsigned int msgID) {
	if(!head || !head->next)
		return 0;

	if (msgID == 0)
		return head->next->msgID;

	head = head->next;
	while (head) {
		if (head->msgID == msgID)
			return head->msgID;
		head = head->next;
	}
	return 0;
}

//Deletes a message with the given ID and returns 1 in the even of success. If the message did not exist or some error occurs, 1
//is returned. If 0 is provided as msgID, then the first element in the list is deleted.
unsigned int delMessage(message* head, unsigned int msgID){
	if (!head || !head->next)
		return 0;

	if (msgID == 0 && head->next) {
		message* temp = head->next;
		head->next = head->next->next;
		free(temp);
		return 1;
	}
	
	//Loop through and find the appropriate msg ID
	while (head->next){
		if (head->next->msgID == msgID) {
			message* temp = head->next;
			head->next = head->next->next;
			free(temp);
			return 1;
		}
		head = head->next;
	}
	return 0;
}
