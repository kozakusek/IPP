#ifndef MY_STRING
#define MY_STRING

#include <stdbool.h>

/* This module introduces dynamic unsigned char array type
 * and a few basic operations */

typedef struct string {
	int size; /* nubmber of memory blocks taken by string */
	int iter; /* first free element */
	unsigned char *name;
}string;

/* String with all values set to 0 constant. */
extern string stringZero;

/* Accepts a ponter to a string "str", and an integer "size".
 * Allocates new memory blocks, creating new string of max length = size.
 * Does not free memory allocated previously! 
 * Returns pointer to a "new" string. */
extern string* stringReset(string* str, int size);

/* Same as stringReset but frees memory. */
extern string* stringHardReset(string* str, int size);

/* Returns true if string under the given pointer is empty. */
extern bool stringIsEmpty(string* str);

/* Adds a new unsigned char "c" to string "*str". 
 * Reallocates memory if more of it is needed.
 * Returns false if memory allocation failure occured. */
extern bool stringPushBack(string* str, unsigned char c);

/* Frees memory allocated for string "*str" 
 * and sets its values to 0. */
extern string* stringClear(string* str);

/* Standard comparing function.
 * Compares "*str"'s value to a given string of chars. */
extern int stringCompareTxt(string* str, char* text);

/* Frees memory exceeding this currently need for
 * proper functioning of string "*str". 
 * Returns pointer to the beggining of "*str"'s value .*/
extern unsigned char* stringCrop(string* str);

/* Accepts pointer to unsigned char "str" and prints it to stdin.
 * Requires it to end with '\0' */
extern void unsignedPrintf(unsigned char* str);

/* Standrad comparing function. 
 * Compares two strings of unsigned chars */
extern int uStrCmp(unsigned char* a, unsigned char* b);


#endif /* MY_STRING */

