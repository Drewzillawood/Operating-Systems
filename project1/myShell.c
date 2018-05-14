#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int stringsInUserInput(char *userInput);

char *removeNewLine(char *commandArgument);

int main(int argc, char **argv)
{	
	int i, status;
	char backgroundName[1024];
        char input[1024];
	char prompt[1024];
	strcpy(prompt, "308cs");
        if(argc > 1)
	{
		strcpy(prompt, argv[1]); 
        }

	printf("%s> ", prompt);
	while(fgets(input, sizeof(input), stdin))
	{
		
		
		char copy[1024];
		strcpy(copy, input);
		char *userInput[stringsInUserInput(input)];
		size_t size = sizeof(userInput) / sizeof(char*);
		userInput[0] = strtok(copy, " ");		
		
		// Getting user input
		for(i = 1; i < size; i++)
		{
			userInput[i] = strtok(NULL, " ");
		}

		// Null terminating character for last element of our string array
		userInput[size] = (char*)NULL;

		if(strcmp(userInput[0], "exit\n") == 0)
		{
			exit(0);
		}
		else if(strcmp(userInput[0], "pid\n") == 0)
		{
			printf("%d\n", getpid());
		}
		else if(strcmp(userInput[0], "ppid\n") == 0)
		{	
			printf("%d\n", getppid());
		}
		else if(strcmp(userInput[0], "cd") == 0)
		{	
			char *removeNL = removeNewLine(userInput[1]);
			chdir(removeNL);
			free(removeNL);
		}
		else if(strcmp(userInput[0], "cd\n") == 0)
		{
			chdir(getenv("HOME"));
		}
		else if(strcmp(userInput[0], "pwd\n") == 0)
		{
			char *pwd = getcwd(NULL, 0);
			printf("%s\n", pwd);
			free(pwd);
		}
		else if(strcmp(userInput[0], "set") == 0)
		{
			if(userInput[1][strlen(userInput[1])-1] == '\n')
			{
				char *removeNL = removeNewLine(userInput[1]);
				unsetenv(removeNL);
				free(removeNL);
			}
			else
			{	
				char *removeNL = removeNewLine(userInput[2]);
				setenv(userInput[1], removeNL, true);
				free(removeNL);		
			}
		} 
		else if(strcmp(userInput[0], "get") == 0)
		{
			char *removeNL = removeNewLine(userInput[1]); 
			bool isNotEmpty = getenv(removeNL);
			if(isNotEmpty)
			{
				printf("%s\n", getenv(removeNL));
			}
			free(removeNL);
		} 
		else
		{	
			int isItMe = fork();
			char *removeNL = removeNewLine(userInput[0]);
			if(isItMe != 0)
			{
				waitpid(isItMe, &status, 0);
				if(size == 1)
				{
					printf("%s Exit %d\n", removeNL, status);
				}
				else 
				{
					printf("%s Exit %d\n", userInput[0], status);
				}
			}
			else
			{	
				if(size == 1)
				{
					printf("\n[%d] %s\n", getpid(), removeNL);
					int temp = execvp(removeNL, userInput); 

					// Should never print
					if(temp)
					{       
                                        	printf("%s: %s\n", removeNL, strerror(errno));
                                      	}

				}
				else
				{	
					printf("Last user Input element: %s\n", userInput[size-1]);
					if(strcmp(userInput[size-1], "&\n") == 0)
					{
						int backgroundProcess = fork();
						if(backgroundProcess == 0)
						{
							userInput[size-1] = (char*)NULL;
							execvp(userInput[0], userInput);
							exit(0);
						}
					} 
					else
					{
						printf("\n[%d] %s\n", getpid(), userInput[0]);
						if(execvp(userInput[0], userInput))
                                        	{
                                               		printf("%s: %s\n", userInput[0], strerror(errno));
                                        	}

					}
     
				}
			
			}
			free(removeNL);
		}
		int backGround = waitpid(-1, &status, WNOHANG);
		while(backGround > 0)
		{
			printf("[%d] Exit %d\n", backGround, status);
			backGround = waitpid(-1, &status, WNOHANG);
		}
		printf("%s> ", prompt);
	}

}

int stringsInUserInput(char *userInput)
{
	int count = 0;
	char *token = strtok(userInput, " ");
	while(token != NULL)
	{
		token = strtok(NULL, " ");
		count++;
	}
	return count;
}

char *removeNewLine(char *commandArgument)
{
	int length = strlen(commandArgument);
	char *ret = malloc(length);
	memcpy(ret, commandArgument, length-1);
	ret[length-1] = '\0';
	return ret;	
}
