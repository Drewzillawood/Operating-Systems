#include "linkedList.h"

LinkedList initialize() 
{
	Node *head      = malloc(sizeof(Node));
	StringArray *s  = malloc(sizeof(StringArray));
	s->strings      = (char**)NULL;
	s->length       = -1;
	head->request   = *s;
	head->next      = head;
	head->prev      = head;
	head->requestId = -1;

	LinkedList *aList = malloc(sizeof(LinkedList));
	pthread_mutex_init(&aList->lock, NULL);
	aList->head   = head;
	aList->length = 0;
	return *aList;
}

void addNode(LinkedList *list, StringArray req, int requestId)
{
	Node *current          = malloc(sizeof(Node));
	current->request       = req;
	list->head->prev->next = current;
	current->prev          = list->head->prev;
	list->head->prev       = current;
	current->next          = list->head;
	current->requestId     = requestId;
	gettimeofday(&current->begin, NULL);
}

void removeNode(LinkedList *list)
{
	if(list->head->next != list->head)
	{
		list->head->next = list->head->next->next;
		list->head->next->prev = list->head;
	}
}