#include "parseString.h"

/*
 *
 * Constant buffer value because I hate
 * having integers anywhere.
 *
 */
static const int BUFFER = 10;

StringArray parseString()
{
	char *input = nextLine(stdin);
	int size = countStrings(input, strlen(input));
	char **inputArray = calloc(size + 1, sizeof(*inputArray));
	char *token = strtok(input, " ");
	inputArray[0] = malloc(strlen(token) + 1);
	strcpy(inputArray[0], token);
	for(int i = 1; i < size - 1; i++)
	{
		token = strtok(NULL, " ");
		inputArray[i] = malloc(strlen(token) + 1);
		strcpy(inputArray[i], token);
	}
	free(input);
	inputArray[size - 1] = (char*)NULL;
	return (StringArray) { .strings = inputArray, .length = size - 1 };
}

int countStrings(char *input, int length)
{
	int count = 0;
	char *tempCopy = malloc(length);
	strcpy(tempCopy, input);
	char* token = strtok(tempCopy, " ");
	while(token)
	{ 
		token = strtok(NULL, " ");
		count++;
	}
	free(tempCopy);
	count++; // Compensate for null terminating element
	return count;
}

char *nextLine(FILE *input)
{
	int buf = BUFFER;
	int read = 0;
	char *buffer = malloc(BUFFER);
	while(true)
	{
		char nextChar = fgetc(input);
		if(nextChar == EOF)
		{
			free(buffer);
			return NULL;
		}
		
		if(read == buf)
		{
			buf *= 2;
			buffer = realloc(buffer, buf);
		}

		if(nextChar == '\n')
		{
			buffer = realloc(buffer, read + 1);
			buffer[buf] = '\0';
			return buffer;
		}

		buffer[read] = nextChar;
		read++;
	}
}