#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "linkedList.h"
#include "Bank.h"

/**  Make file included, simply type 'make' **/

LinkedList      *requests;
Account         **accounts;
pthread_cond_t  worker_cv;

int main(int argc, char *argv[])
{
	if(argc != 4)
	{
		exit(0);
	}
	UserInput user = (UserInput)
	{
		.numWorkers  = atoi(argv[1]),
		.numAccounts = atoi(argv[2]),
		.fileName    = argv[3]
	};

	/* Initialize mutex and conditional variables */
	pthread_cond_init(&worker_cv, NULL);
	initialize_accounts(user.numAccounts);
	accounts = calloc(user.numAccounts, sizeof(Account));
	for (int i = 0; i < user.numAccounts; i++)
	{
		Account *acct = malloc(sizeof(Account));
		pthread_mutex_init(&acct->lock, NULL);
		acct->accountId = i + 1;
		accounts[i] = acct;
	}
	/**********************************************/

	/* Create our main thread */
	pthread_t mThread;
	pthread_create(&mThread, NULL, mainThread, (void*)&user);

	/* Wait until our main thread is done */
	pthread_join(mThread, NULL);

	// Free our LinkedList from memory
	for (int i = 0; i < user.numAccounts; i++)
	{
		free(accounts[i]);
	}
	free(accounts);
}

void *mainThread(void *arg)
{
	UserInput user = (UserInput) 
	{
    	.numWorkers  = ((UserInput*)arg)->numWorkers, 
		.numAccounts = ((UserInput*)arg)->numAccounts, 
		.fileName    = ((UserInput*)arg)->fileName 
	};
	LinkedList hold = initialize();
	requests = &hold;
	/* Create our worker thread array */
	pthread_t wThread[((UserInput*)arg)->numWorkers];
	for (int i = 0; i < ((UserInput*)arg)->numWorkers; i++)
	{
		pthread_create(&wThread[i], NULL, workerThread, (void*)&user);
	}
	int requestId = 1;
	while(true)
	{
		printf("> ");
		StringArray newArgs = parseString();
		printf("ID %d\n", requestId);
		if(strcmp(newArgs.strings[0], "END") == 0)
		{
			pthread_mutex_lock(&requests->lock);
			for (int i = 0; i < user.numWorkers; i++)
			{
				addNode(requests, newArgs, requestId);
			}
			pthread_cond_broadcast(&worker_cv);
			pthread_mutex_unlock(&requests->lock);
			break;
		}
		else
		{
			pthread_mutex_lock(&requests->lock);
			addNode(requests, newArgs, requestId);
			pthread_cond_broadcast(&worker_cv);
			pthread_mutex_unlock(&requests->lock);
			requestId++;
		}
	}
	for (int i = 0; i < user.numWorkers; i++)
	{
		pthread_join(wThread[i], NULL);
	}
	free(&requests->head->request);
	pthread_exit(0);
}

void *workerThread(void *arg)
{
	while(true)
	{ 
		pthread_mutex_lock(&requests->lock);
		while(requests->head->next->request.length == -1)
			pthread_cond_wait(&worker_cv, &requests->lock);
		Node *threadNode = requests->head->next;
		struct timeval end;
		removeNode(requests);
		if(strcmp(threadNode->request.strings[0], "CHECK") == 0)
		{
			int currentId = atoi(threadNode->request.strings[1]);
			if(currentId < ((UserInput*)arg)->numAccounts)
			{
				pthread_mutex_lock(&accounts[currentId]->lock);
				FILE *output;
				output = fopen(((UserInput*)arg)->fileName, "a");
				gettimeofday(&end, NULL);
				fprintf(output, "%d BAL %d TIME %d.%06d %d.%06d\n", threadNode->requestId, read_account(currentId), (int)threadNode->begin.tv_sec, (int)threadNode->begin.tv_usec, (int)end.tv_sec, (int)end.tv_usec);
				fclose(output);
				pthread_mutex_unlock(&accounts[currentId]->lock);
			}
		}
		else if (strcmp(threadNode->request.strings[0], "TRANS") == 0)
		{
			int tAccts = threadNode->request.length / 2;
			tAccts = (tAccts == 0) ? 1 : tAccts;
			int accts[tAccts];
			int ammnt[tAccts];
			int ok = -1;
			for (int i = 0, j = 1; i < tAccts; i++, j++)
			{
				accts[i] = atoi(threadNode->request.strings[j]) - 1;
				ammnt[i] = atoi(threadNode->request.strings[j+1]);
				j++;
			}
			for(int i = 0; i < tAccts; i++)
			{
				pthread_mutex_lock(&accounts[accts[i]]->lock);
				if(read_account(accts[i]) + ammnt[i] < 0 || accts[i] > ((UserInput*)arg)->numAccounts)
				{
					ok = accts[i];
				}
				else
				{
					ammnt[i] += read_account(accts[i]);
				}
			}
			for (int i = 0; i < tAccts; i++)
			{
				if(ok < 0)
				{
					write_account(accts[i], ammnt[i]);
				}
				pthread_mutex_unlock(&accounts[accts[i]]->lock);
			}
			FILE *output;
			output = fopen(((UserInput*)arg)->fileName, "a");
			gettimeofday(&end, NULL);
			if(ok > 0)
			{
				fprintf(output, "%d ISF %d TIME %d.%06d %d.%06d\n", threadNode->requestId, ok, (int)threadNode->begin.tv_sec, (int)threadNode->begin.tv_usec, (int)end.tv_sec, (int)end.tv_usec);
			}
			else
			{
				fprintf(output, "%d OK TIME %d.%0d %d.%06d\n", threadNode->requestId, (int)threadNode->begin.tv_sec, (int)threadNode->begin.tv_usec, (int)end.tv_sec, (int)end.tv_usec);
			}
			fclose(output);
		}
		else if (strcmp(threadNode->request.strings[0], "END") == 0)
		{
			free(threadNode);
			pthread_mutex_unlock(&requests->lock);
			pthread_exit(0);
		}
		free(threadNode);
		pthread_mutex_unlock(&requests->lock);
	}
}