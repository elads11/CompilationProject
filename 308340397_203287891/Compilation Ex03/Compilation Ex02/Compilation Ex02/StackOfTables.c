#include "StackOfTables.h"


TableNode* currNode = NULL;

TableNode* insertTableNode()
{
	TableNode* newTable = (TableNode*)malloc(sizeof(TableNode));
	newTable->table = createTree();
	newTable->next = currNode;
	currNode = newTable;
	return newTable;
}

TableNode* deleteCurrentNode(){
	TableNode *temp = currNode;
	currNode = currNode->next;
	free(temp);
	return currNode;
}

TableNode* getCurrentNode(){
	return currNode;
}

ElementAttribute* searchAllScopes(char *key){
	TableNode *temp = currNode;
	Tree *table;
	ElementAttribute *elementAttribute;
	while(temp != NULL){
		table = temp->table;
		elementAttribute = searchInTree(&table, key);
		if (elementAttribute != NULL)
			return elementAttribute;
		else
			temp = temp->next;
	}

	return NULL;
}