#ifndef STACKOFTABLES_H
#define STACKOFTABLES_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "BinaryTree.h"

typedef struct TableNode
{
	Tree* table;
	struct TableNode* next;
}TableNode;


TableNode* insertTableNode();
TableNode* deleteCurrentNode();
TableNode* getCurrentNode();
ElementAttribute* searchAllScopes(char *key);

#endif