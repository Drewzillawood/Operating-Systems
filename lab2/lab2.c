#include <stdio.h>

int main(int argc, char** argv) {

	printf("Process ID is: %d\n", getpid());
	printf("Parent process ID is %d\n", getppid());
	sleep(60); /*sleeps for two mintues*/
	printf("I am awake.\n");
	return 0;

}
