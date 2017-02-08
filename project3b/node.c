#include "node.h"

//Adds a message to the given linked list head. Returns true if add was successful, false if add failed
int addMessage(message* head, message* msg){
	if (!head || !msg)
		return 0; //idiot check

	//Find the end of the linked list
	while (head->next)
		head = head->next;

	head->next = msg;
	return 1;
}

//Returns message of given ID using the given linked list head. If 0 is passed in for an ID, the first message will be returned.
//Returns null on failure.
message* getMessage(message* head, unsigned int msgID) {
	if (msgID == 0 && head)
		return head->next;

	head = head->next;
	while (head) {
		if (head->msgID == msgID)
			return head;
		head = head->next;
	}
	return NULL;
}

//Deletes a message with the given ID and returns 1 in the even of success. If the message did not exist or some error occurs, 1
//is returned. If 0 is provided as msgID, then the first element in the list is deleted.
//NOTE: THE MESSAGE IS NOT FREED, IT IS THE USER'S RESPOSIBILITY TO FREE() MESSAGES
int delMessage(message* head, unsigned int msgID){
	if (!head || !head->next)
		return 0;

	if (msgID == 0 && head->next)
		head = head->next->next;
	
	//Loop through and find the appropriate msg ID
	message* prev = head;
	head = head->next;
	while (head){
		if (head->msgID == msgID){
			prev->next = head->next;
			return 1;
		}
		heads = head->next;
		prev = prev->next;
	}
	return 0;
}
