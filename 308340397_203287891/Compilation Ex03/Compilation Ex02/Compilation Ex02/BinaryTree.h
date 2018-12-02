#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>


typedef struct ElementAttribute {
	char* name;
	char* role;
	char* type;
	char* category;
	char* subtype;
	int size;
} ElementAttribute;

typedef struct TreeNode{
	char* key;
	ElementAttribute* value;
	struct TreeNode *right;
	struct TreeNode	*left;
}TreeNode;

typedef struct Tree{
	TreeNode* root;
	/*struct Tree* parent;*/
	/*struct Tree* child;*/
}Tree;


ElementAttribute* createElementAttribute();
Tree* createTree(/*Tree *parent*/);
void insertIntoTree(Tree** tree, char* key, ElementAttribute* value);
ElementAttribute* searchInTree(Tree** tree, char* key);
void printTree(Tree** tree);

#endif