/* Bartlomiej Kozaryna
 * nr: bk______
 * Male zadanie  forests */

#include <stdlib.h>
#include <stdio.h>
#include "mystring.h"
#include "mytree.h"
#include "myforests.h"

#define OPERATION_MAX_LEN 5
#define MAX_COUNT_OF_INSTRUCTIONS 4


static inline bool isProperWhiteChar(unsigned char c) {
	return c == 32 || c == 9 || c == 11 || c == 12 || c == 13;
}

static inline void resetTStrings(oper, s1, s2, s3, ignore)
 string* oper;
 string* s1; 
 string* s2; 
 string* s3; 
 bool ignore; 
{
	if (stringCompareTxt(oper, "ADD") == 0 && !ignore) {
		if (!stringIsEmpty(s1))
			s1 = stringReset(s1, 0);
		if (!stringIsEmpty(s2))
			s2 = stringReset(s2, 0);
		if (!stringIsEmpty(s3))
			s3= stringReset(s3, 0);
	}
	else {
		if (!stringIsEmpty(s1))
			s1 = stringHardReset(s1, 0);
		if (!stringIsEmpty(s2))
			s2 = stringHardReset(s2, 0);
		if (!stringIsEmpty(s3))
			s3 = stringHardReset(s3, 0);
	}

	oper = stringHardReset(oper, OPERATION_MAX_LEN);
}

int main() {
	Tree world = NULL;
	string operation, forest, tree, animal;
	string* instructions[4] = {&operation, &forest, &tree, &animal};
	int whichPhrase = 0; /* iterator for instructions */
	short input;
	unsigned char tempChar, prevChar = '\0';
	bool ignore = false; /* indicates if current line is being ignored */
	bool firstInLine = true; 
	bool failed = false; /* necessery for "#" corner case */

	/* strings' initialisation */
	operation = *stringReset(&operation, OPERATION_MAX_LEN);
	forest = *stringReset(&forest, 0);
	tree = *stringReset(&tree, 0);
	animal = *stringReset(&animal, 0);

	/* input processor */
	while ((input = getc(stdin)) != EOF) {
		tempChar = (unsigned char)input;
		if (tempChar == '\n') {
			/* if input line was correct */
			if (!ignore) {
				/* if line was not empty */
				if (!stringIsEmpty(&operation)) {
					if (!isCorrectOperation(operation)) {
						ignore = true;
						fprintf(stderr ,"ERROR\n");
					}
					else {
						if (!performOperation(&world, &operation, &forest,
											&tree, &animal))
							exit(1); /* not enough memory for new nodes */
					}
				}
			}
			/* reset all variables */
			resetTStrings(&operation, &forest, &tree, &animal, ignore);
			whichPhrase = 0;
			ignore = false;
			firstInLine = true;
			failed = false;
		}
		else if (ignore) {
			/* ignoring current line */
		}
		else if (firstInLine) { 
			if (tempChar == '#') {
				ignore = true;
			}
			else if (33 <= tempChar) {
				stringPushBack(&operation, tempChar);
			}
			else if (!isProperWhiteChar(tempChar)) {
				/* unallowed input */
				fprintf(stderr ,"ERROR\n");
				ignore = true;
				failed = true;
			}

			firstInLine = false;
		}
		else if (33 <= tempChar) {
			if (whichPhrase < MAX_COUNT_OF_INSTRUCTIONS) {
				if (!stringPushBack((instructions[whichPhrase]), tempChar))
					exit(1); /* not enough memory for current string */ 
			}
			else {
				/* too many instructions in the current lane */
				fprintf(stderr ,"ERROR\n");
				ignore = true;
				failed = true;
			}
		}
		else if (isProperWhiteChar(tempChar)) {
			/* proceed to processing next instruction */
			if (!isProperWhiteChar(prevChar))
				whichPhrase++;
		}
		else {
			/* unallowable input */
			fprintf(stderr ,"ERROR\n");
			ignore = true;
			failed = true;
		}

		prevChar = tempChar;
	}

	/* last line was not clear and did not end with '\n'
	 * and ERROR was not raised before */
	if (prevChar != '\n' && !stringIsEmpty(&operation) && !failed)
		fprintf(stderr ,"ERROR\n");

	/* free all allocated memory */
	stringClear(&operation);
	stringClear(&forest);
	stringClear(&tree);
	stringClear(&animal);
	removeAll(&world);

	return 0;
}
