#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

void *thread1();
void *thread2();

int main(int argc, char* argv[])
{
	// Thread 1
	pthread_t i1;
	pthread_create(&i1, NULL, thread1, NULL);
	// Thread 2
	pthread_t i2;
	pthread_create(&i2, NULL, thread2, NULL);
	// Joining of threads with main
	//pthread_join(i2, NULL);
	//pthread_join(i1, NULL);
	printf("I AM A GTX 10-80!\n"); 
}

// Our Thread1 Function
void *thread1() {
	// sleep added to demontsrate usefulness of join
	sleep(5);
	printf("Hey! Thread1 here!\n");
}

// Identical to Thread2
void *thread2() {
	sleep(5);
	printf("Hey! Thread2 here!\n");
}
