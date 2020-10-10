#ifndef MY_TREE
#define MY_TREE

#include "mystring.h"

/* This module introduces the AVL tree type, with value of 
 * unsigned char string and keeping another AVL tree (here by name "subtree")
 * (distinct from left and rigth subtrees - children) 
 * and a few basic procedures. */

struct Node;

typedef struct Node* Tree;

struct Node {
	unsigned char* name; 
	Tree subtree; 
	Tree left, right;
	int height;
	int bf; /* Balance Factor */
};

/* Removes all elemets from a given tree "*t". */
extern void removeAll(Tree* t);

/* Extracts string value from "*x" and adds a new node to tree "*t" 
 * taking exact address of "*x" value.
 * Controls two extern variables: 
 * failure - indicating memory allocation failure
 * padded - indicating if a node with same value already existed in "*t". */
extern Tree insert(Tree* t, string* x, bool* failure, bool* padded);

/* Removes node with value "*x", as well as its subtree
 * Frees all allocated memory */
extern Tree removeElement(Tree* t, unsigned char* x);

/* Returns a node named "x" if it exists in tree "t"
 * elsewise returned value is NULL */
extern Tree mem(Tree t, unsigned char* x);

/* Same as mem but returns pointer to wanted tree */
extern Tree* memPointer(Tree* t, unsigned char* x);

/* Prints all node values from a given tree in infix order */
extern void printAll(Tree t);


#endif /* MY_TREE */

