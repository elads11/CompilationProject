#include "Token.h"

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int currentIndex = 0;
Node* currentNode = NULL;
int isPreviousBackToken = 0;
int tokenIndex = 0;
#define TOKEN_ARRAY_SIZE 1000

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{ 
	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if(currentNode == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->tokensArray = (Token*)malloc(sizeof(Token)*TOKEN_ARRAY_SIZE);
		if(currentNode->tokensArray == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if(currentNode == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray = (Token*)malloc(sizeof(Token)*TOKEN_ARRAY_SIZE);

			if(currentNode->tokensArray == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}

	currentNode->tokensArray[currentIndex].kind = kind;
	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char)*(strlen(lexeme)+1));
	strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;
}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token *back_token() { 
	
	Token *theToken = NULL;
	if (!currentNode)
		return NULL;
	
	if (tokenIndex > 0)
	{
		tokenIndex--;
		isPreviousBackToken = 1;
	}
	else //tokenIndex == 0
	{
		if (currentNode->prev)
		{
			currentNode = currentNode->prev;
			tokenIndex = TOKEN_ARRAY_SIZE - 1;
			isPreviousBackToken = 1;
		}
		else
		{
			return NULL;
		}
	}

	theToken = &(currentNode->tokensArray[tokenIndex]);

	return theToken;
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token): 
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/
Token *next_token() { 
	Token *theNextToken = NULL;
	if (!currentNode || isPreviousBackToken == 0)
	{
		yylex();
	}

	theNextToken = &(currentNode->tokensArray[tokenIndex]);
	
	if (tokenIndex < TOKEN_ARRAY_SIZE - 1)
	{
		tokenIndex++;
	}
	else
	{
		currentNode = currentNode->next;
		tokenIndex = 0;
	}

	isPreviousBackToken = 0;

	return theNextToken;
}