#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*
 *
 * Compact String array, contains length attribute
 *
 */
typedef struct {
	char **strings;
	int  length;
} StringArray;

/*
 *
 * Function to parse user input
 *
 */
StringArray parseString();

/*
 *
 * Determines number of strings from user input
 *
 */
int countStrings(char *input, int length);

/*
 *
 * Standardized nextLine, allocates exact amount of memory
 * from specified input
 *
 */
char *nextLine(FILE *input);