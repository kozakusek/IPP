#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mystring.h"

#define STRING_START_LEN 10 

string stringZero = {0, 0, NULL};

string* stringReset(string* str, int size) {
	if (size > 0)
		str->size = size;
	else 
		str->size = STRING_START_LEN;

	str->name = calloc((str->size) + 1, sizeof(unsigned char));
	str->iter = 0;

	return str;
}

string* stringHardReset(string* str, int size) {
	free(str->name);

	return stringReset(str, size);
}

bool stringIsEmpty(string* str) {
	if (str == NULL)
		return true;
	else if (str->name == NULL)
		return true;
	else if ((str->name)[0] == '\0')
		return true;
	else 
		return false; 
}

bool stringPushBack(string* str, unsigned char c) { 
	if (str->iter == str->size - 1) {
		str->size *= 2;
		str->name = realloc(str->name,(str->size+1)*sizeof(unsigned char));
		if (str->name == NULL)
			return false;

		memset(str->name+str->iter+1,'\0', 1+(str->size)/2);
	}

	(str->name)[str->iter] = c;
	(str->iter)++;

	return true;
}

string* stringClear(string* str) {
	free(str->name);
	str->iter = 0;
	str->size = 0;

	return str;
}

int stringCompareTxt(string* str, char* text) {
	unsigned char* p1 = str->name;
	unsigned char* p2 = (unsigned char*)text;

	while ((*p1) && (*p1 == *p2)) {
		p1++;
		p2++;
	}
	
	return (*p1 - *p2);
}

unsigned char* stringCrop(string* str) {
	str->size = str->iter + 1;
	str->name = realloc(str->name, (str->size)*sizeof(unsigned char));

	return str->name;
}

void unsignedPrintf(unsigned char* str) {
	unsigned char* temp = str;

	while (*temp != '\0'){
		putchar(*temp);
		temp++;
	}
	printf("\n");	
}

int uStrCmp(unsigned char* a, unsigned char* b) {
	unsigned char* temp1 = a;
	unsigned char* temp2 = b;

	while ((*temp1) && (*temp1 == *temp2)) {
		temp1++;
		temp2++;
	}

	return (*temp1 - *temp2);
}

