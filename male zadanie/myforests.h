#ifndef MY_TREES
#define MY_TREES

#include "mytree.h"

/* This module introduces key operations for "Male zadanie" - forests. */

// TODO tutaj lepiej byłoby nazwać funkcję `isCorrectOperationName` zamiast
//  dawać wyjaśniający komentarz
/* Checks only if operation name is correct. */
bool isCorrectOperation(string oper);

/* Performs the "ADD" operation on a given world.
 * Adds tree structured creation: 
 * forest->tree->anmial. 
 * Returns false in case of memory allocation problems. */
bool add(Tree* world, string* forest, string* tree, string* animal);

/* Performs the "DEL" oepration on a given world.
 * Deletes from it last existing element of structure:
 * forest->tree->animal. */
void delete(Tree* world, string* forest, string* tree, string* animal);

/* Performs the "PRINT" operation on a given world.
 * Prints in-order all elements of the last existing element of:
 * world->forest->tree */
void print(Tree* world, string forest, string tree);

/* Performs the "CHECK" operation on a given world.
 * Checks if given object forest->tree->animal exists in "*world".
 * Accepts string = "*", mening "all". */
bool check(Tree* world, string* forest, string* tree, string* animal);

/* Checks if operation "*op" is executable on arguments "*fo", "*tr", "*an"
 * and if yes performs it on "*wo".
 * Returns false in case of memory allocation problems in procedure "add". */
bool performOperation(Tree* wo, string* op, string* fo, string* tr, string* an);


#endif /* MY_TREES */

