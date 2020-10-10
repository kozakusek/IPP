#include <stdlib.h>
#include <stdio.h>
#include "myforests.h"

bool isCorrectOperation(string oper) {
	if (stringCompareTxt(&oper, "ADD") == 0 
			|| stringCompareTxt(&oper, "DEL") == 0)
		return true;
	else if (stringCompareTxt(&oper, "PRINT") == 0 
				|| stringCompareTxt(&oper, "CHECK") == 0)
		return true;
	else 
		return false;
}

bool add(Tree* world, string* forest, string* tree, string* animal) {
	// TODO null pointer is already checked in stringIsEmpty
    /* last case of reccurence */
	if (forest == NULL || stringIsEmpty(forest)) {
		return true;
	}
	else {
		bool failure = false; /* Could insert allocate memory */
		bool padded = false; /* Had an object of given name already existed */

		insert(world, forest, &failure, &padded);
		if (failure)
			return false;

		Tree t = mem((*world), forest->name); /* cannot be NULL */

		if (padded)
			stringHardReset(forest, 0);

		if (!add(&(t->subtree), tree, animal, NULL))
			return false;
		}

	return true;
}

void delete(Tree* world, string* forest, string* tree, string* animal) {
	if ((*world) == NULL) {
		return;
	}
	else {
		if (forest == NULL || stringIsEmpty(forest)) {
			removeAll(world);
			(*world) = NULL;
		}
		else {
			Tree* t = memPointer(world, forest->name);
			if (t == NULL)
				return; /* no item to delete */

			if ((*t) != NULL)
				t = &((*t)->subtree);

			delete(t, tree, animal, NULL); 
			if (tree == NULL || stringIsEmpty(tree))
				(*world) = removeElement(world, forest->name);
		}
	}
}

void print(Tree* world, string forest, string tree) {
	if ((*world) == NULL) {
		return;
	}
	else {
		/* possible correct combinations of input: 
		 * None*None; Some*None; Some*Some */
		if (stringIsEmpty(&forest) && stringIsEmpty(&tree)) {
			printAll(*world);
		}
		else {
			Tree trees = mem((*world), forest.name);
			if (trees != NULL) 
				trees = trees->subtree;
			
			if (stringIsEmpty(&tree)) {
				printAll(trees);
			}
			else {
				Tree animals = mem(trees, tree.name);
				if (animals != NULL) 
					animals = animals->subtree;

				printAll(animals); 
			}
		}
	}
} 

bool check(Tree* world, string* forest, string* tree, string* animal) { 
	if ((*world) == NULL) {
		return false;
	}
	else {
		/* we happily abuse recursive structure of the problem */
		if (stringCompareTxt(forest, "*") == 0) {
			if (check(&((*world)->subtree), tree, animal, &stringZero)) 
				return true;
			else if (check(&((*world)->left), forest, tree, animal))
				return true;
			else
				return check(&((*world)->right), forest, tree, animal);
		}
		else {
			Tree t;
			/* forest of given name does not exist in our world */
			if ((t = mem(*world, forest->name)) == NULL) {
				return false;
			}
			/* forest of given name does exists in our world */
			else {
				/* we do not need to look further */
				if (stringIsEmpty(tree)) {
					return true;
				}
				/* we are supposed to seek further*/
				else if ((t = t->subtree) == NULL) {
					return false; /* there are no trees in out forest */
				}
				/* we look for tree in our forest using 
				 *recursive structure of the problem */
				else {
					return check(&t, tree, animal, &stringZero);
				}
			}
		}
	}

} 


static inline bool isAStar(string* str) {
	if (stringCompareTxt(str, "*") == 0)
		return true;
	else 
		return false;
}

bool performOperation(Tree* wo, string* op, string* fo, string* tr, string* an) {
	if (stringCompareTxt(op, "ADD") == 0) {
		bool key = true;
		if (!stringIsEmpty(fo)) {
			key = add(wo, fo, tr, an); /* perform "ADD" */ 
			printf("OK\n");
		}
		else {
			fprintf(stderr ,"ERROR\n");
		}

		return key; 
	}
	else if (stringCompareTxt(op, "DEL") == 0) {
		delete(wo, fo, tr, an); /*perform "DEL" */
		printf("OK\n");

		return true;
	}
	else if (stringCompareTxt(op, "PRINT") == 0) {
		if (stringIsEmpty(an)) {
			print(wo, *fo, *tr); /* perform "PRINT" */
		}
		else {
			/* animal variable must be NULL */
			fprintf(stderr ,"ERROR\n");
		}

		return true;
	}
	else if (stringCompareTxt(op, "CHECK") == 0) { 
		if (!isAStar(an)) {
			if (stringIsEmpty(an)) {
				if (!isAStar(tr)) {
					if (stringIsEmpty(tr)) {
						if (isAStar(fo) || stringIsEmpty(fo)) {
							/* CHECK cannot be executed on no variables 
							 * and also a single one cannot be equal to "*" */
							fprintf(stderr ,"ERROR\n");
						}
						else {
							if (check(wo, fo, tr, an)) /* perform "CHECK" */
								printf("YES\n"); 
							else 
								printf("NO\n");
						}
					}
					else {
						if (check(wo, fo, tr, an)) /* perform "CHECK" */
							printf("YES\n"); 
						else 
							printf("NO\n");
					}
				}
				else {
					/* last variable can never be equal to "*" */
					fprintf(stderr ,"ERROR\n");
				}
			}
			else {
				if (check(wo, fo, tr, an)) /* perform CHECK */
					printf("YES\n"); 
				else 
					printf("NO\n");
			}
		}
		else {
			/* animal variable can never be equal to "*" */
			fprintf(stderr ,"ERROR\n");
		}

		return true;
	}

	return false;
}

