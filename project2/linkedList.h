#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "parseString.h"

/*
 *
 * Simple struct to hold user Input
 * from *argv[]
 *
 */
typedef struct 
{
	int  numWorkers;
	int  numAccounts;
	char *fileName;
} UserInput;

/*
 *
 * Account implementation
 *
 */
typedef struct
{
	pthread_mutex_t lock;
	int accountId;
} Account;

/*
 *
 * Initial Node declaration
 *
 */
typedef struct Node Node;

/*
 *
 * Node implementation for lists
 *
 */
struct Node
{
	StringArray    request;
	Node           *next;
	Node           *prev;
	int            requestId;
	struct timeval begin;
};

/*
 *
 * Linked list implementation
 * Intended to be circular until further notice
 *
 */
typedef struct 
{
	pthread_mutex_t lock;
	Node *head;
	int  length;
} LinkedList;

/*
 *
 * Initializes all Nodes and their
 * respective accounts
 *
 */
LinkedList initialize();

/*
 *
 * Add a node to the linked list
 *
 */
void addNode(LinkedList *list, StringArray req, int requestId);

/*
 *
 * Remove the Node with the specified acctId
 *
 */
void removeNode(LinkedList *list);

void *mainThread(void *arg);

void *workerThread(void *arg);

