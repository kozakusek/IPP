#include <stdio.h>
#include <stdlib.h>
#include "mytree.h"


static inline int max(int a, int b) {
	return a > b ? a : b;
}	


static inline int compare(unsigned char* a, unsigned char* b) {
	int key = uStrCmp(a, b);
	if (key == 0) 
		return 0;
	else if (key > 0)
		return 1;
	else 
		return -1;
}

void removeAll(Tree* t) {
	if ((*t) != NULL) {
		removeAll(&((*t)->left));
		removeAll(&((*t)->right));
		removeAll(&((*t)->subtree));
		free((*t)->name);
	}
	free((*t));
	(*t) = NULL;
}

/* Refreshes height and balance factor of given node "*t" */
static inline void update(Tree* t) {
	int leftNodeHeight = ((*t)->left == NULL) ? -1 : ((*t)->left)->height; 
	int rightNodeHeight = ((*t)->right == NULL) ? -1 : ((*t)->right)->height;
	
	(*t)->height = 1 + max(leftNodeHeight, rightNodeHeight);
	(*t)->bf = rightNodeHeight - leftNodeHeight;
}

/* Implementation of typical AVL tree rotations */

static inline Tree rightRotation(Tree* t) {
	Tree newParent = (*t)->left;
	(*t)->left = newParent->right;
	newParent->right = (*t);

	update(t);
	update(&newParent);

	return newParent;
}

static inline Tree leftRotation(Tree* t) {
	Tree newParent = (*t)->right;
	(*t)->right = newParent->left;
	newParent->left = (*t);

	update(t);
	update(&newParent);

	return newParent;
}

static inline Tree leftLeftCase(Tree* t) {
	return rightRotation(t);
}

static inline Tree leftRightCase(Tree* t) {
	(*t)->left = leftRotation(&((*t)->left));

	return rightRotation(t);
}

static inline Tree rightRightCase(Tree* t) {
	return leftRotation(t);
}

static inline Tree rightLeftCase(Tree* t) {
	(*t)->right = rightRotation(&((*t)->right));

	return leftRotation(t);
}

static Tree balance(Tree* t) {
	/* Left cases */
	if ((*t)->bf == -2) {
		/* Left-left case */
		if (((*t)->left)->bf <= 0) {
			(*t) = leftLeftCase(t);
		}
		/* Left-right case */
		else {
			(*t) = leftRightCase(t);
		}
	}
	/* Right cases */
	else if ((*t)->bf == 2) {
		/* Right-right case */
		if (((*t)->right)->bf >= 0) {
			(*t) = rightRightCase(t);
		}
		/* Right-left case */
		else {
			(*t) = rightLeftCase(t);
		}
	}

	return (*t);
}

Tree insert(Tree* t, string* x, bool* failure, bool* padded) {
	if ((*t) == NULL) {
		(*t) = calloc(1, sizeof(struct Node));
		if ((*t) == NULL)
			(*failure) = true;
		else
			(*t)->name = stringCrop(x);

		return (*t);
	}
	
	int key = compare((*t)->name, x->name); 
	
	if (key == 0) {
		(*padded) = true;
		return (*t);
	} 
	else if (key == 1) {
		(*t)->left = insert(&((*t)->left), x, failure, padded);
	} 
	else {
		(*t)->right = insert(&((*t)->right), x, failure, padded);
	}
	
	update(t);
		
	return balance(t);
}

/* Returns left-most node */
static inline Tree findMin(Tree* t) {
	Tree tmp = (*t);
	while(tmp->left != NULL)
		tmp = tmp->left;

	return tmp;
}

/* Returns right-most node*/
static inline Tree findMax(Tree* t) {
	Tree tmp = (*t);
	while(tmp->right != NULL)
		tmp = tmp->right;

	return tmp;	
}

static inline void swapTreeData(Tree* t1, Tree* t2) {
	unsigned char* tempName = (*t1)->name;
	Tree tempSubtree = (*t1)->subtree;
	(*t1)->name = (*t2)->name;
	(*t1)->subtree = (*t2)->subtree;
	(*t2)->name = tempName;
	(*t2)->subtree = tempSubtree;
}

Tree removeElement(Tree* t, unsigned char* x) {
	if ((*t) == NULL) 
		return NULL;

	int key = compare((*t)->name, x);

	if (key == 1) {
		(*t)->left = removeElement(&((*t)->left), x);
	}
	else if (key == -1) {
		(*t)->right = removeElement(&((*t)->right), x);
	}
	else {
		if ((*t)->left == NULL){
			Tree tmp = (*t)->right;
			removeAll(&((*t)->subtree));
			free((*t)->name);
			free(*t);
			(*t) = NULL;

			return tmp;
		}
		else if ((*t)->right == NULL) {
			Tree tmp = (*t)->left;
			removeAll(&((*t)->subtree));
			free((*t)->name);
			free(*t);
			(*t) = NULL;

			return tmp;
		}
		else {
			if (((*t)->right)->height < ((*t)->left)->height) {
				Tree successor = findMax(&((*t)->left));

				swapTreeData(&successor, t);

				(*t)->left = removeElement(&((*t)->left), x);
			}
			else {
				Tree successor = findMin(&((*t)->right));

				swapTreeData(&successor, t);
				
				(*t)->right = removeElement(&((*t)->right), x);
			}
		}	
	}

	update(t);

	return balance(t);
}

Tree mem(Tree t, unsigned char* x) {
	if (t == NULL || x == NULL) 
		return NULL;

	int key = compare(t->name, x);

	if (key == 0) 
		return t;
	else if (key == -1) 
		return mem(t->right, x);
	else 
		return mem(t->left, x);
}

Tree* memPointer(Tree* t, unsigned char* x) {
	if (*t == NULL || x == NULL) 
		return NULL;

	int key = compare((*t)->name, x);

	if (key == 0)
		return t;
	else if (key == -1)
		return memPointer(&((*t)->right), x);
	else
		return memPointer(&((*t)->left), x);
}

void printAll(Tree t) {
	if (t == NULL)
		return;

	printAll(t->left);
	unsignedPrintf(t->name);
	printAll(t->right);
}

