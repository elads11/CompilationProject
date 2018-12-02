#include "BinaryTree.h"

void insert(TreeNode *node, char* key, ElementAttribute* value);
TreeNode* search(TreeNode *node, char* key);

ElementAttribute* createElementAttribute(){
	ElementAttribute* elemAttr = (ElementAttribute*)malloc(sizeof(ElementAttribute));
	elemAttr->name = "";
	elemAttr->role = "";
	elemAttr->type = "";
	elemAttr->category = "";
	elemAttr->subtype = "";
	elemAttr->size = 0;
	return elemAttr;
}

Tree* createTree(/*Tree* parent*/){
	Tree* tree = (Tree*)malloc(sizeof(Tree));
	tree->root = NULL;
	/*if (parent){
		parent->child = tree;
		tree->parent = parent;
	}
	else
		tree->parent = NULL;*/
	return tree;
}

void insertIntoTree(Tree **tree, char* key, ElementAttribute* value){
	insert(&((*tree)->root), key, value);
}

ElementAttribute* searchInTree(Tree** tree, char* key){
	TreeNode* result;
	if(!(*tree) || !((*tree)->root))
		return NULL;
	result = search(&((*tree)->root), key);
	if (result != NULL)
		return result->value;
	return NULL;
}

void insert(TreeNode **node, char* key, ElementAttribute* value)
{
	int result;
	TreeNode *temp = NULL;
	if ((*node) == NULL)
	{
		temp = (TreeNode*)malloc(sizeof(TreeNode));
		temp->left = temp->right = NULL;
		temp->key = key;
		temp->value = value;
		*node = &(*temp);
		return;
	}
	result = strcmp(key, (*node)->key);
	if (result < 0)
	{
		insert(&((*node)->left), key, value);
	}
	else if (result > 0)
	{
		insert(&((*node)->right), key, value);
	}
}

TreeNode* search(TreeNode **node, char* key)
{
	int result;

	if (*node == NULL)
	{
		return NULL;
	}
	else{
		result = strcmp(key, (*node)->key);
		if (result < 0)
		{
			return search(&((*node)->left), key);
		}
		else if (result > 0)
		{
			return search(&((*node)->right), key);
		}
		else
		{
			return *node;
		}
	}
}

void printSubtree(TreeNode **tn){
	if (tn == NULL)
		return;
	if ((*tn)->left)
		printSubtree(&((*tn)->left));
	printf((*tn)->key);
	if ((*tn)->right)
		printSubtree(&((*tn)->right));
}

void printTree(Tree** tree){
	TreeNode* root = (*tree)->root;
	if (tree == NULL || root == NULL)
		printf("Tree is empty");
	else
		printSubtree(&root);
}