#include "parser.h"

typedef struct compound
{
	char* response;
	char*  descripition;
}compound;

void parseProgram();
void parseBlock();
void parseDefinitions();
void parseDefinitionsTag();
void parseDefinition();
void parseVarDefinition();
ElementAttribute* parseVarDefinitionTag();
void parseTypeDefinition();
ElementAttribute* parseTypeIndicator();
char* parseBasicType();
ElementAttribute* parseArrayType();
ElementAttribute* parsePointerType();
char* parsePointerTypeTag();
int parseSize();
void parseCommands();
void parseCommandsTag();
void parseCommand();
compound* parseReceiver();
compound* parseReceiverTag();
compound* parseExpression();
compound* parseExpressionTag();

void match(eTOKENS expected);
void printRule(char* str);
void printErrorMessageMatch(eTOKENS expected, Token *actual);
void printErrorMessageParse(char** expectedKinds, int arraySize, Token *actual);
void printErrorMessageSemantic(char* message, int lineNumber);

compound * createCompound();

char* ERROR_TYPE = "error_type";
char* SUCCESS = "OK";




TableNode* tableNode = NULL;
Tree* table = NULL;
Token *t = NULL;
FILE* f = NULL;
FILE* f_semantic = NULL;

void runParser(char* outputFile, char* semanticOutputFile){
	f = fopen(outputFile, "w+");
	f_semantic = fopen(semanticOutputFile, "w+");
	parseProgram();
	match(TOKEN_EOF);
	fclose(f_semantic);
	fclose(f);
}

void parseProgram(){
	t = next_token();
	switch(t->kind){
	case TOKEN_KEYWORD_BLOCK: 
		printRule("PROGRAM -> BLOCK");
		t = back_token();
		parseBlock();
		break;
	default:
	{
		t = back_token();		
		//printErrorMessageParse(arr, 1, t);
	}
	}
}

void parseBlock(){
	//char* arr[] = { "TOKEN_KEYWORD_BLOCK" };
	tableNode = insertTableNode();
	table = tableNode->table;
	t = next_token();
	switch(t->kind){
	case TOKEN_KEYWORD_BLOCK:
		printRule("BLOCK -> block DEFINITIONS; begin COMMANDS; end");	
		parseDefinitions();
		match(TOKEN_SEPARATOR_SIGN_SEMICOLON);
		match(TOKEN_KEYWORD_BEGIN);
		parseCommands();
		match(TOKEN_SEPARATOR_SIGN_SEMICOLON);
		match(TOKEN_KEYWORD_END);
		tableNode = deleteCurrentNode();
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 1, t);
	}
	}
}

void parseDefinitions(){
	//char* arr[] = { "TOKEN_ID", "TOKEN_KEYWORD_TYPE" };
	t = next_token();
	switch(t->kind){
	case TOKEN_ID:
	case TOKEN_KEYWORD_TYPE:
		printRule("DEFINITIONS -> DEFINITION DEFINITIONS_TAG");
		t = back_token();
		parseDefinition();
		parseDefinitionsTag();
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 2, t);
	}
	}
}

void parseDefinitionsTag(){
	//char* arr[] = { "TOKEN_SEPARATOR_SIGN_SEMICOLON", "TOKEN_ID", "TOKEN_KEYWORD_BEGIN" };
	t = next_token();
	switch(t->kind){
	case TOKEN_SEPARATOR_SIGN_SEMICOLON:
		t = next_token();
		if(t->kind == TOKEN_KEYWORD_BEGIN)
		{
			printRule("DEFINITIONS_TAG -> {EPSILON}");
			t = back_token();
			t = back_token();
			break;
		}
		else
		{
			printRule("DEFINITIONS_TAG -> ; DEFINITION DEFINITIONS_TAG");
			t = back_token();
			parseDefinition();
			parseDefinitionsTag();
			break;
		}
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 3, t);
	}
	}
}

void parseDefinition(){
	//char* arr[] = { "TOKEN_ID", "TOKEN_KEYWORD_TYPE"};
	t = next_token();
	switch(t->kind){
	case TOKEN_ID:
		printRule("DEFINITION -> VAR_DEFINITION");
		t = back_token();
		parseVarDefinition();
		break;
	case TOKEN_KEYWORD_TYPE:
		printRule("DEFINITION -> TYPE_DEFINITION");
		t = back_token();
		parseTypeDefinition();
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 2, t);
	}
	}
}

void parseVarDefinition(){
	//char* arr[] = { "TOKEN_SEPARATOR_SIGN_COLON" };
	ElementAttribute* elementAttribute = createElementAttribute();
	char *key;
	int lineNumber;
	t = next_token();
	switch(t->kind){
	case TOKEN_ID:
		printRule("VAR_DEFINITION -> id : VAR_DEFINITION_TAG");
		key = t->lexeme;
		lineNumber = t->lineNumber;
		match(TOKEN_SEPARATOR_SIGN_COLON);
		elementAttribute = parseVarDefinitionTag();
		if ((strcmp(elementAttribute->type, "integer") != 0) && (strcmp(elementAttribute->type, "real") != 0) && (searchAllScopes(elementAttribute->type) == NULL))
		{
			printErrorMessageSemantic("Error: Illegal variable type. Please use one that exists. Line %d", lineNumber);
		}
		else
		{
			if(strcmp(elementAttribute->role, ERROR_TYPE) != 0)
			{
				elementAttribute->role = "variable";
				elementAttribute->name = key;
				if(searchInTree(&table, key) != NULL)
					printErrorMessageSemantic("Error: Variable already defined. Line %d", lineNumber);
				else
					insertIntoTree(&table, key, elementAttribute);  
			}
		}
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 1, t);
	}
	}
}

ElementAttribute* parseVarDefinitionTag(){
	//char* arr[] = { "TOKEN_ID", "TOKEN_KEYWORD_INTEGER", "TOKEN_KEYWORD_REAL"};
	ElementAttribute* elementAttribute = createElementAttribute();
	char* type;
	t = next_token();
	switch(t->kind){
	case TOKEN_ID:
		printRule("VAR_DEFINITION_TAG -> id");
		elementAttribute->type = t ->lexeme;
		return elementAttribute;
		break;
	case TOKEN_KEYWORD_INTEGER:
	case TOKEN_KEYWORD_REAL:
		printRule("VAR_DEFINITION_TAG -> BASIC_TYPE");
		t = back_token();
		type = parseBasicType();
		if (strcmp(type, ERROR_TYPE) == 0){
			printErrorMessageSemantic("Error: Illegal basic type. Line %d", t->lineNumber);
			elementAttribute->role = ERROR_TYPE;
		}
		else
			elementAttribute->type = type;
		return elementAttribute;
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 3, t);
		elementAttribute -> role = ERROR_TYPE; 
		return elementAttribute;
	}
	}
}

void parseTypeDefinition(){
	//char* arr[] = { "TOKEN_KEYWORD_TYPE" };
	ElementAttribute* elementAttribute = createElementAttribute();
	char* name;
	int line, idExists = 0;
	t = next_token();
	switch(t->kind){
	case TOKEN_KEYWORD_TYPE:
		printRule("TYPE_DEFINITION -> type id is TYPE_INDICATOR");	
		match(TOKEN_ID);
		name = t->lexeme;
		line = t->lineNumber;
		match(TOKEN_KEYWORD_IS);
		elementAttribute = parseTypeIndicator();
		if(strcmp(elementAttribute->role, ERROR_TYPE) != 0){		
			elementAttribute->name = name;
			if (searchAllScopes(name) != NULL) 
				printErrorMessageSemantic("Error: type already exists. Line %d ", t->lineNumber);
			else
				insertIntoTree(&table, name, elementAttribute);
		}

		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 1, t);
	}
	}
}

ElementAttribute* parseTypeIndicator(){
	//char* arr[] = { "TOKEN_KEYWORD_INTEGER", "TOKEN_KEYWORD_REAL", "TOKEN_KEYWORD_ARRAY", "TOKEN_POINTER" };
	char* val;
	ElementAttribute* elementAttribute = createElementAttribute();
	t = next_token();
	switch(t->kind){
	case TOKEN_KEYWORD_INTEGER:
	case TOKEN_KEYWORD_REAL:
		printRule("TYPE_INDICATOR -> BASIC_TYPE");
		t = back_token();
		val = parseBasicType();
		if (strcmp(val, ERROR_TYPE) == 0){
			printErrorMessageSemantic("Error: Illegal basic type. Line %d", t->lineNumber);
			elementAttribute->role = ERROR_TYPE;
		}
		else{
			elementAttribute->role = "user_defined_type";
			elementAttribute->category = "basic";
			if (strcmp(val, "integer") == 0)
				elementAttribute->subtype = "integer";
			else if (strcmp(val, "real") == 0)
				elementAttribute->subtype = "real";
		}
		return elementAttribute;
		break;
	case TOKEN_KEYWORD_ARRAY:
		printRule("TYPE_INDICATOR -> ARRAY_TYPE");	
		t = back_token();
		elementAttribute = parseArrayType();
		if(strcmp(elementAttribute->role, ERROR_TYPE) != 0)
			elementAttribute->role = "user_defined_type";
		return elementAttribute;
		break;
	case TOKEN_POINTER:
		printRule("TYPE_INDICATOR -> POINTER_TYPE");
		t = back_token();
		elementAttribute = parsePointerType();
		if(strcmp(elementAttribute->role, ERROR_TYPE) != 0)
			elementAttribute->role = "user_defined_type";
		return elementAttribute;
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 4, t);
		elementAttribute->role = ERROR_TYPE;
		return elementAttribute;
	}
	}
}

char* parseBasicType(){
	//char* arr[] = { "TOKEN_KEYWORD_INTEGER", "TOKEN_KEYWORD_REAL" };
	t = next_token();
	switch(t->kind){
	case TOKEN_KEYWORD_INTEGER:
		printRule("BASIC_TYPE -> integer");
		return "integer";
		break;
	case TOKEN_KEYWORD_REAL:
		printRule("BASIC_TYPE -> real");
		return "real";
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 2, t);
		return ERROR_TYPE;
	}
	}
}

ElementAttribute* parseArrayType(){
	//char* arr[] = { "TOKEN_KEYWORD_ARRAY" };
	int size;
	char* basicType;
	ElementAttribute* elementAttribute = createElementAttribute();
	t = next_token();
	switch(t->kind){
	case TOKEN_KEYWORD_ARRAY:
		printRule("BASIC_TYPE -> array [SIZE] of BASIC_TYPE");
		match(TOKEN_SEPARATOR_SIGN_LB);
		size = parseSize();
		match(TOKEN_SEPARATOR_SIGN_RB);
		match(TOKEN_KEYWORD_OF);
		basicType = parseBasicType();
		if (strcmp(basicType, ERROR_TYPE) == 0){
			printErrorMessageSemantic("Error: Illegal basic type. Line %d", t->lineNumber);
			elementAttribute->role = ERROR_TYPE;
		}
		else
			elementAttribute->subtype = basicType;
		if(size == 0){
			printErrorMessageSemantic("Error: Size must be larger than 0. Line: %d", t->lineNumber);
			elementAttribute->role = ERROR_TYPE;
		}
		else
			elementAttribute->size = size;
		elementAttribute->category = "array";
		return elementAttribute;
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 1, t);
		elementAttribute->role = ERROR_TYPE;
		return elementAttribute;
	}
	}
}

ElementAttribute* parsePointerType(){
	//char* arr[] = { "TOKEN_POINTER" };
	char* subtype;
	ElementAttribute* elementAttribute = createElementAttribute();
	t = next_token();
	switch(t->kind){
	case TOKEN_POINTER:
		printRule("POINTER_TYPE -> ^POINTER_TYPE_TAG");
		subtype = parsePointerTypeTag();
		if(strcmp(subtype, ERROR_TYPE) == 0)
			elementAttribute->role = ERROR_TYPE;
		else{
			elementAttribute->subtype = subtype;
			elementAttribute->category = "pointer";
		}
		return elementAttribute;
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 1, t);
		elementAttribute->role = ERROR_TYPE;
		return elementAttribute;
	}
	}
}

char* parsePointerTypeTag(){
	//char* arr[] = { "TOKEN_ID", "TOKEN_KEYWORD_INTEGER", "TOKEN_KEYWORD_REAL" };
	char* subtype;
	ElementAttribute* elementAttribute;
	t = next_token();
	switch(t->kind){
	case TOKEN_ID:
		printRule("POINTER_TYPE_TAG -> id");
		elementAttribute = searchInTree(&table, t->lexeme);
		if (elementAttribute == NULL){
			printErrorMessageSemantic("Error: Type Undefined. Line %d", t->lineNumber);
			return ERROR_TYPE;
		}
		else{
			subtype = elementAttribute->name;
			if(subtype == NULL){
				printErrorMessageSemantic("Error: Pointer to undefined type. Line %d", t->lineNumber);
				return ERROR_TYPE;
			}
			else
				return subtype;
		}
		break;
	case TOKEN_KEYWORD_INTEGER:
	case TOKEN_KEYWORD_REAL:
		printRule("POINTER_TYPE_TAG -> BASIC_TYPE");
		t = back_token();
		subtype = parseBasicType();
		if (strcmp(subtype, ERROR_TYPE) == 0)
			printErrorMessageSemantic("Error: Illegal basic type. Line %d", t->lineNumber);
		return subtype;
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 3, t);
		return ERROR_TYPE;
	}
	}
}

int parseSize(){
	//char* arr[] = { "TOKEN_INT_NUM" };
	int size;
	t = next_token();
	switch(t->kind){
	case TOKEN_INT_NUM:
		printRule("SIZE -> int_num");
		size=atoi(t->lexeme);
		return size;
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 1, t);
		return 0;
	}
	}
}

void parseCommands(){
	//char* arr[] = { "TOKEN_ID", "TOKEN_KEYWORD_WHEN", "TOKEN_KEYWORD_FOR", "TOKEN_KEYWORD_FREE", "TOKEN_KEYWORD_BLOCK" };
	t = next_token();
	switch(t->kind){
	case TOKEN_ID:
	case TOKEN_KEYWORD_WHEN:
	case TOKEN_KEYWORD_FOR:
	case TOKEN_KEYWORD_FREE:
	case TOKEN_KEYWORD_BLOCK:
		printRule("COMMANDS -> COMMAND COMMANDS_TAG");
		t = back_token();
		parseCommand();
		parseCommandsTag();
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 5, t);
	}
	}
}

void parseCommandsTag(){
	//char* arr[] = { "TOKEN_SEPARATOR_SIGN_SEMICOLON" };
	t = next_token();
	switch(t->kind){
	case TOKEN_SEPARATOR_SIGN_SEMICOLON:
		t = next_token();
		if ((t->kind == TOKEN_KEYWORD_DEFAULT) || (t->kind == TOKEN_KEYWORD_END) || (t->kind == TOKEN_KEYWORD_END_WHEN) || (t->kind == TOKEN_KEYWORD_END_FOR))
		{
			printRule("COMMANDS_TAG -> {EPSILON}");
			t = back_token();
			t = back_token();
			break;
		}
		else
		{				
			printRule("COMMANDS_TAG -> ; COMMAND COMMANDS_TAG");
			t = back_token();
			parseCommand();
			parseCommandsTag();
			break;
		}
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 1, t);
	}
	}
}

void parseCommand(){
	//char* arr[] = { "TOKEN_ID", "TOKEN_KEYWORD_WHEN", "TOKEN_KEYWORD_FOR", "TOKEN_KEYWORD_FREE", "TOKEN_KEYWORD_BLOCK" };
	ElementAttribute* leftSideID = NULL;
	ElementAttribute* rightSideID = NULL;
	compound * comp1 = NULL;
	compound * comp2 = NULL;
	char* tableValue1; //helper
	char* tableValue2; //helper
	int lineNumber;
	int isUserDefinedTypeReceiver = 0;
	char* lexeme;
	t = next_token();
	switch(t->kind){
	case TOKEN_ID:
		lexeme = t->lexeme;
		lineNumber = t->lineNumber;
		leftSideID = searchAllScopes(lexeme);
		t = next_token();
		if (t->kind == TOKEN_OP_ASSIGNMENT)
		{
			t = next_token();
			if (t->kind == TOKEN_KEYWORD_MALLOC)
			{
				printRule("COMMAND -> id = malloc(size_of(id))");
				match(TOKEN_SEPARATOR_SIGN_LP);
				match(TOKEN_KEYWORD_SIZE_OF);
				match(TOKEN_SEPARATOR_SIGN_LP);
				match(TOKEN_ID);
				rightSideID = searchInTree(&table, t->lexeme);
				match(TOKEN_SEPARATOR_SIGN_RP);
				match(TOKEN_SEPARATOR_SIGN_RP);
				if(leftSideID != NULL && rightSideID != NULL){
					if (strcmp(leftSideID->role, "variable") != 0)
						printErrorMessageSemantic("Error: Cannot use operations on non-variables. Line %d", lineNumber);
					else if (strcmp(rightSideID->role, "user_defined_type") != 0)
						printErrorMessageSemantic("Error: Malloc can only be used on types. Line %d", lineNumber);
					else{ //first ID is variable, 2nd ID is user defined type
						tableValue1 = searchAllScopes(leftSideID->type)->category;
						tableValue2 = searchAllScopes(leftSideID->type)->subtype;
						if (strcmp(tableValue1, "pointer") != 0)
							printErrorMessageSemantic("Error: Left hand side of malloc operation must be a pointer type. Line %d", lineNumber);
						else if (strcmp(tableValue2, rightSideID->name) != 0)
								printErrorMessageSemantic("Error: Attempting to use malloc on different type. Line %d", lineNumber);
					}
				}			
				else
					printErrorMessageSemantic("Error: Variable undefined. Line %d", lineNumber);
				break;
			}
			else
			{
				printRule("COMMAND -> RECEIVER = EXPRESSION");
				t = back_token();
				t = back_token();
				t = back_token();
				comp1 = parseReceiver();
				match(TOKEN_OP_ASSIGNMENT);
				comp2 = parseExpression();
				if (strcmp(comp1->response, ERROR_TYPE) != 0)
				{
					if(leftSideID != NULL && strcmp(comp1->response,ERROR_TYPE) != 0 && strcmp(comp2->response,ERROR_TYPE) != 0){ //receiver ID found in table
						if (strcmp(leftSideID->role, "variable") != 0) //left side is not a variable
							printErrorMessageSemantic("Error: Left side expression must be a variable. Line %d", t->lineNumber);
						else if (strcmp(comp2->descripition, "basic") != 0 && strcmp(comp2->descripition, "pointer") != 0 && strcmp(comp2->descripition, "array") != 0) //right side type is not basic type
							printErrorMessageSemantic("Error: Right side expression must be a basic type. Line %d", t->lineNumber);
						else {
							if (strcmp(leftSideID->type, "integer") == 0 || strcmp(leftSideID->type, "real") == 0) {
								if(strcmp(comp1->response, comp2->response) != 0 || strcmp(comp1->descripition, comp2->descripition) != 0)
									printErrorMessageSemantic("Error: Different types in assignment. Line %d", t->lineNumber);
							}
							else {
								rightSideID = searchAllScopes(leftSideID->type); //type of left side
								if (rightSideID == NULL)
									printErrorMessageSemantic("Error: Left side expression must be a variable of a basic or defined type. Line %d", t->lineNumber);
								else if (strcmp(rightSideID->role, "user_defined_type") == 0){
									if(strcmp(rightSideID->category, "array") == 0){ //left side is array
										if (strcmp(comp1->descripition, "array") == 0)
											printErrorMessageSemantic("Error: Left side expression cannot be array type. Line %d", t->lineNumber);
									}
									else if(strcmp(comp1->response, comp2->response) != 0 || strcmp(comp1->descripition, comp2->descripition) != 0)
										printErrorMessageSemantic("Error: Different types in assignment. Line %d", t->lineNumber);
								}
							}
						}
					}
				}
				break;
			}
		}
		else 
		{
			printRule("COMMAND -> RECEIVER = EXPRESSION");
			t = back_token();
			t = back_token();
			comp1 = parseReceiver();
			match(TOKEN_OP_ASSIGNMENT);
			comp2 = parseExpression();
			lineNumber = t->lineNumber;
			if (strcmp(comp1->response, ERROR_TYPE) != 0)
			{
				if(leftSideID != NULL && strcmp(comp1->response,ERROR_TYPE) != 0 && strcmp(comp2->response,ERROR_TYPE) != 0){ //receiver ID found in table
					if (strcmp(leftSideID->role, "variable") != 0) //left side is not a variable
						printErrorMessageSemantic("Error: Left side expression must be a variable. Line %d", t->lineNumber);
					else if (strcmp(comp2->descripition, "basic") != 0 && strcmp(comp2->descripition, "pointer") != 0 && strcmp(comp2->descripition, "array") != 0) //right side type is not basic type
						printErrorMessageSemantic("Error: Right side expression must be a basic type. Line %d", t->lineNumber);
					else {
						if (strcmp(leftSideID->type, "integer") == 0 || strcmp(leftSideID->type, "real") == 0) {
							if(strcmp(comp1->response, comp2->response) != 0 || strcmp(comp1->descripition, comp2->descripition) != 0)
								printErrorMessageSemantic("Error: Different types in assignment. Line %d", t->lineNumber);
						}
						else {
							rightSideID = searchAllScopes(leftSideID->type); //type of left side
							if (rightSideID == NULL)
								printErrorMessageSemantic("Error: Left side expression must be a variable of a basic or defined type. Line %d", t->lineNumber);
							else if (strcmp(rightSideID->role, "user_defined_type") == 0){
								if(strcmp(rightSideID->category, "array") == 0){ //left side is array
							        if (strcmp(comp1->descripition, "array") == 0)
										printErrorMessageSemantic("Error: Left side expression cannot be array type. Line %d", t->lineNumber);
								}
								else if(strcmp(comp1->response, comp2->response) != 0 || strcmp(comp1->descripition, comp2->descripition) != 0)
									printErrorMessageSemantic("Error: Different types in assignment. Line %d", t->lineNumber);
							}
						}
					}
				}
			}
		}
		break;
	case TOKEN_KEYWORD_WHEN:
		printRule("COMMAND -> when (EXPRESSION rel_op EXPRESSION) do COMMANDS; default COMMANDS; end_when");
		match(TOKEN_SEPARATOR_SIGN_LP);
		comp1 = parseExpression();
		match(TOKEN_REL_OP);
		comp2 = parseExpression();
		lineNumber = t->lineNumber;
		if(strcmp(comp1->descripition, "basic") != 0 || strcmp(comp2->descripition, "basic") != 0)
			printErrorMessageSemantic("Error: relative operator must be between basic types. Line %d", t->lineNumber);
		else if (strcmp(comp1->response, comp2->response) != 0)
			printErrorMessageSemantic("Error: relative operator must be between similar types. Line %d", t->lineNumber);
		match(TOKEN_SEPARATOR_SIGN_RP);
		match(TOKEN_KEYWORD_DO);
		parseCommands();
		match(TOKEN_SEPARATOR_SIGN_SEMICOLON);
		match(TOKEN_KEYWORD_DEFAULT);
		parseCommands();
		match(TOKEN_SEPARATOR_SIGN_SEMICOLON);
		match(TOKEN_KEYWORD_END_WHEN);
		break;
	case TOKEN_KEYWORD_FOR:		
		printRule("COMMAND -> for (id = EXPRESSION; id rel_op EXPRESSION; id++) COMMANDS; end_for");
		match(TOKEN_SEPARATOR_SIGN_LP);
		match(TOKEN_ID);
		searchAllScopes(t->lexeme);
		match(TOKEN_OP_ASSIGNMENT);
		parseExpression();
		match(TOKEN_SEPARATOR_SIGN_SEMICOLON);
		match(TOKEN_ID);
		match(TOKEN_REL_OP);
		parseExpression();
		match(TOKEN_SEPARATOR_SIGN_SEMICOLON);
		match(TOKEN_ID);
		match(TOKEN_OP_INC);
		match(TOKEN_SEPARATOR_SIGN_RP);
		parseCommands();
		match(TOKEN_SEPARATOR_SIGN_SEMICOLON);
		match(TOKEN_KEYWORD_END_FOR);
		break;
	case TOKEN_KEYWORD_FREE:		
		printRule("COMMAND -> free(id)");
		match(TOKEN_SEPARATOR_SIGN_LP);
		match(TOKEN_ID);
		leftSideID = searchAllScopes(t->lexeme);
		lineNumber = t->lineNumber;
		match(TOKEN_SEPARATOR_SIGN_RP);
		if (leftSideID == NULL)
			printErrorMessageSemantic("Error: Variable undefined. Line %d", t->lineNumber);
		else if (strcmp(leftSideID->role, "variable") != 0)
			printErrorMessageSemantic("Error: can only free a variable. Line %d", t->lineNumber);
		break;
	case TOKEN_KEYWORD_BLOCK:		
		printRule("COMMAND -> BLOCK");
		t = back_token();
		parseBlock();
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 5, t);
	}
	}
}

compound* parseReceiver(){
	//char* arr[] = { "TOKEN_ID" };
	compound * comp = createCompound();
	compound * val;
	char * lexeme;
	int lineNumber;
	ElementAttribute *elementAttribute = NULL;
	char * tableValue1; //helper variable
	char * tableValue2; //helper variable
	t = next_token();
	switch(t->kind){
	case TOKEN_ID:
		printRule("RECEIVER -> id RECEIVER_TAG");
		lexeme = t->lexeme;
		elementAttribute = searchAllScopes(lexeme);
		lineNumber = t->lineNumber;
		val = parseReceiverTag();
		if(strcmp(val->response, ERROR_TYPE) == 0)
			return val;
		else {
			if (elementAttribute != NULL){
				if(strcmp(val->descripition, "index") == 0){
					if(strcmp(elementAttribute->role, "variable") == 0){
						if (strcmp(elementAttribute->type, "integer") == 0 || strcmp(elementAttribute->type, "real") == 0){
							comp->response = elementAttribute->type;
							comp->descripition = "basic";
							return comp;
						}
						tableValue1 = searchAllScopes(elementAttribute->type)->category;  
						tableValue2 = searchAllScopes(elementAttribute->type)->subtype;  
						if (strcmp(tableValue1, "array") == 0){
							comp->response = tableValue2;
							if (strcmp(tableValue2, "integer") == 0 || strcmp(tableValue2, "real") == 0)
								comp->descripition = "basic";
							else
								comp->descripition = "pointer";
						}
						else{
							printErrorMessageSemantic("Error: Can only use this kind of operation on array types. Line %d", lineNumber);
							comp->response = ERROR_TYPE;
						}
					}
					else{
						printErrorMessageSemantic("Error: Identifier not a variable. Line %d", lineNumber);
						comp->response = ERROR_TYPE;
					}
				}
				else if(strcmp(val->descripition, "pointer") == 0){
					if(strcmp(elementAttribute->role, "variable") == 0){
						if (strcmp(elementAttribute->type, "integer") == 0 || strcmp(elementAttribute->type, "real") == 0){
							comp->response = elementAttribute->type;
							comp->descripition = "basic";
							return comp;
						}
						tableValue1 = searchAllScopes(elementAttribute->type)->category;
						tableValue2 = searchAllScopes(elementAttribute->type)->subtype;
						if (strcmp(tableValue1, "pointer") == 0){
							comp->response = tableValue2;
							if (strcmp(tableValue2, "integer") == 0 || strcmp(tableValue2, "real") == 0)
								comp->descripition = "basic";
							else
								comp->descripition = "pointer";
						}
						else{
							printErrorMessageSemantic("Error: Can only use this kind of operation on pointer types. Line %d", lineNumber);
							comp->response = ERROR_TYPE;
						}
					}
					else{
						printErrorMessageSemantic("Error: Identifier not a variable. Line %d", lineNumber);
						comp->response = ERROR_TYPE;
					}
				}
				else if(strcmp(val->descripition, "") == 0){ //RECEIVER_TAG -> {EPSILON} 
					if (strcmp(elementAttribute->role, "variable") != 0)
						printErrorMessageSemantic("Error: Identifier not a variable. Line %d", lineNumber);
					else {
						if (strcmp(elementAttribute->type, "integer") == 0 || strcmp(elementAttribute->type, "real") == 0){
							comp->response = elementAttribute->type;
							comp->descripition = "basic";
						}
						else{
							tableValue1 = searchAllScopes(elementAttribute->type)->category;
							tableValue2 = searchAllScopes(elementAttribute->type)->subtype;
							comp->response = tableValue2;
							comp->descripition = tableValue1;
						}
					}
				}
			}
			else{
				printErrorMessageSemantic("Error: Variable undefined. Line %d", lineNumber);
				comp->response = ERROR_TYPE;
			}
			return comp;
		}
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 1, t);
		comp->response = ERROR_TYPE;
		return comp;
	}
	}
}

compound* parseReceiverTag(){
	//char* arr[] = { "TOKEN_SEPARATOR_SIGN_LB", "TOKEN_OP_ASSIGNMENT", "TOKEN_POINTER" };
	compound * comp = createCompound();
	compound * val;
	int lineNumber;
	ElementAttribute* elementAttribute = NULL;
	t = next_token();
	switch(t->kind)
	{
	case TOKEN_SEPARATOR_SIGN_LB:
		printRule("RECEIVER_TAG -> [EXPRESSION]");
		val = parseExpression();
		lineNumber = t->lineNumber;
		match(TOKEN_SEPARATOR_SIGN_RB);
		if(strcmp(val->response, "integer") == 0 && strcmp(val->descripition, "basic") == 0)
		{
			comp->response = "integer";
			comp->descripition = "index";
			return comp;
		}
		else{
			printErrorMessageSemantic("Error: array index must be of type integer. Line %d", lineNumber);
			comp->response = ERROR_TYPE;
			return comp;
		}
		break;
	case TOKEN_OP_ASSIGNMENT:
		printRule("RECEIVER_TAG -> {EPSILON}");
		t = back_token();
		return comp;
		break;
	case TOKEN_POINTER:
		printRule("RECEIVER_TAG -> ^");
		comp->descripition = "pointer";
		return comp;
		break;
	default:
	{
		t = back_token();
		comp->response = ERROR_TYPE;
		return comp;
		//printErrorMessageParse(arr, 3, t);
	}
	}
}

compound* parseExpression()
{
	//char* arr[] = { "TOKEN_INT_NUM", "TOKEN_REAL_NUM", "TOKEN_ID", "TOKEN_ADDRESS", "TOKEN_KEYWORD_SIZE_OF" };
	compound * comp = createCompound();
	compound * val;
	char * lexeme;
	int lineNumber;
	ElementAttribute *elementAttribute = NULL;
	char * tableValue1; //helper variable
	char * tableValue2; //helper variable
	t = next_token();
	switch(t->kind){
	case TOKEN_INT_NUM:
		printRule("EXPRESSION -> int_num");
		comp->response = "integer";
		comp->descripition = "basic";
		return comp;
		break;
	case TOKEN_REAL_NUM:
		printRule("EXPRESSION -> real_num");
		comp->response = "real";
		comp->descripition = "basic";
		return comp;
		break;
	case TOKEN_ID:
		printRule("EXPRESSION -> id EXPRESSION_TAG");
		lexeme = t->lexeme;
		elementAttribute = searchAllScopes(lexeme);
		lineNumber = t->lineNumber;
		val = parseExpressionTag();
		if (strcmp(val->response, ERROR_TYPE) == 0)
			return val;
		else{
			if (elementAttribute != NULL){
				if(strcmp(val->descripition, "basic") == 0){
					if (strcmp(elementAttribute->type, "integer") == 0 || strcmp(elementAttribute->type, "real") == 0){
						if (strcmp(elementAttribute->type, val->response) == 0){
							comp->response = elementAttribute->type;
							comp->descripition = "basic";
						}
						else{
							printErrorMessageSemantic("Error: Type mismatch in operation. Line %d", lineNumber);
							comp->response = ERROR_TYPE;
						}
					}
					else{ //ar_op with a non-basic type variable at left side of the operation
						tableValue1 = searchAllScopes(elementAttribute->type)->category;
						tableValue2 = searchAllScopes(elementAttribute->type)->subtype;
						if (strcmp(tableValue1, "basic") == 0){
							if (strcmp(tableValue2, val->response) == 0){
								comp->response = tableValue2;
								comp->descripition = "basic";
							}
							else{
								printErrorMessageSemantic("Error: Type mismatch in operation. Line %d", lineNumber);
								comp->response = ERROR_TYPE;
							}
						}
						else{ //non basic user defined type of variable
							printErrorMessageSemantic("Error: Can only use arithmetic operation between basic types. Line %d", lineNumber);
							comp->response = ERROR_TYPE;
						}
					}
				}
				else if(strcmp(val->descripition, "index") == 0){
					if(strcmp(elementAttribute->role, "variable") ==0){
						if (strcmp(elementAttribute->type, "integer") == 0 || strcmp(elementAttribute->type, "real") == 0){
							comp->response = elementAttribute->type;
							comp->descripition = "basic";
							return comp;
						}
						tableValue1 = searchAllScopes(elementAttribute->type)->category;  
						tableValue2 = searchAllScopes(elementAttribute->type)->subtype;  
						if (strcmp(tableValue1, "array") == 0){
							comp->response = tableValue2;
							if (strcmp(tableValue2, "integer") == 0 || strcmp(tableValue2, "real") == 0)
								comp->descripition = "basic";
							else
								comp->descripition = "pointer";
						}
						else{
							printErrorMessageSemantic("Error: Can only use this kind of operation on array types. Line %d", lineNumber);
							comp->response = ERROR_TYPE;
						}
					}
					else{
						printErrorMessageSemantic("Error: Identifier not a variable. Line %d", lineNumber);
						comp->response = ERROR_TYPE;
					}
				}
				else if(strcmp(val->descripition, "pointer") == 0){
					if(strcmp(elementAttribute->role, "variable") ==0){
						if (strcmp(elementAttribute->type, "integer") == 0 || strcmp(elementAttribute->type, "real") == 0){
							comp->response = elementAttribute->type;
							comp->descripition = "basic";
							return comp;
						}
						tableValue1 = searchAllScopes(elementAttribute->type)->category;  
						tableValue2 = searchAllScopes(elementAttribute->type)->subtype;  
						if (strcmp(tableValue1, "pointer") == 0){
							comp->response = tableValue2;
							if (strcmp(tableValue2, "integer") == 0 || strcmp(tableValue2, "real") == 0)
								comp->descripition = "basic";
							else
								comp->descripition = "pointer";
						}
						else{
							printErrorMessageSemantic("Error: Can only use this kind of operation on pointer types. Line %d", lineNumber);
							comp->response = ERROR_TYPE;
						}
					}
					else{
						printErrorMessageSemantic("Error: Identifier not a variable. Line %d", lineNumber);
						comp->response = ERROR_TYPE;
					}
				}
				else if(strcmp(val->descripition, "") == 0){ //EXPRESSION_TAG -> {EPSILON} 
					if (strcmp(elementAttribute->role, "variable") != 0)
						printErrorMessageSemantic("Error: Identifier not a variable. Line %d", lineNumber);
					else {
						if (strcmp(elementAttribute->type, "integer") == 0 || strcmp(elementAttribute->type, "real") == 0){
							comp->response = elementAttribute->type;
							comp->descripition = "basic";
						}
						else{
							tableValue1 = searchAllScopes(elementAttribute->type)->category;
							tableValue2 = searchAllScopes(elementAttribute->type)->subtype;
							comp->response = tableValue2;
							comp->descripition = tableValue1;
						}
					}
				}
			}
			else{
				printErrorMessageSemantic("Error: Variable undefined. Line %d", lineNumber);
				comp->response = ERROR_TYPE;
			}
			return comp;
		}
		break;
	case TOKEN_ADDRESS:
		printRule("EXPRESSION -> &id");
		match(TOKEN_ID);
		lineNumber = t->lineNumber;
		lexeme = t->lexeme;
		elementAttribute = searchAllScopes(lexeme);
		if (elementAttribute != NULL){
			if (strcmp(elementAttribute->role, "variable") == 0){
				comp->response = elementAttribute->type;
				comp->descripition = "pointer";
				return comp;
			}
			else
			{
				printErrorMessageSemantic("Error: Address can only be used with a variable. Line %d", lineNumber);
				comp->response = ERROR_TYPE;
				return comp;
			}
		}
		else{
			printErrorMessageSemantic("Error: Variable undefined. Line %d", lineNumber);
			comp->response = ERROR_TYPE;
			return comp;
		}
		break;
	case TOKEN_KEYWORD_SIZE_OF:
		printRule("EXPRESSION -> size_of(id)");
		match(TOKEN_SEPARATOR_SIGN_LP);
		match(TOKEN_ID);
		lexeme = t->lexeme;
		lineNumber = t->lineNumber;
		match(TOKEN_SEPARATOR_SIGN_RP);
		elementAttribute = searchAllScopes(lexeme);
		if (elementAttribute != NULL){
			if (strcmp(elementAttribute->role, "user_defined_type") != 0){
				printErrorMessageSemantic("Error: Variable undefined. Line %d", lineNumber);
				comp->response = ERROR_TYPE;
				return comp;
			}
			else{
				comp->response = "integer";
				comp->descripition = "basic";
				return comp;
			}
		}
		else
		{
			printErrorMessageSemantic("Error: Variable undefined. Line %d", lineNumber);
			comp->response = ERROR_TYPE;
			return comp;
		}
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 5, t);
		comp->response = ERROR_TYPE;
		return comp;
	}
	}
}

compound* parseExpressionTag(){
	//char* arr[] = { "TOKEN_SEPARATOR_SIGN_LB", "TOKEN_SEPARATOR_SIGN_RB", "TOKEN_SEPARATOR_SIGN_RP", "TOKEN_SEPARATOR_SIGN_SEMICOLON", "TOKEN_AR_OP", "TOKEN_REL_OP", "TOKEN_POINTER" };
	compound* comp = createCompound();
	compound* val;
	int lineNumber;
	t = next_token();
	switch(t->kind){
	case TOKEN_SEPARATOR_SIGN_LB:
		printRule("EXPRESSION_TAG -> [EXPRESSION]");
		val = parseExpression();
		lineNumber = t->lineNumber;
		match(TOKEN_SEPARATOR_SIGN_RB);
		if(strcmp(val->response, "integer") == 0 && strcmp(val->descripition, "basic") == 0)
		{
			comp->response = "integer";
			comp->descripition = "index";
			return comp;
		}
		else{
			printErrorMessageSemantic("Error: array index must be of type integer. Line %d", lineNumber);
			comp->response = ERROR_TYPE;
			return comp;
		}
		break;
	case TOKEN_AR_OP:
		printRule("EXPRESSION_TAG -> ar_op EXPRESSION");
		val = parseExpression();
		lineNumber = t->lineNumber;
		if(strcmp(val->response, "integer") == 0 && strcmp(val->descripition, "basic") == 0){
			comp->response = "integer";
			comp->descripition = "basic";
		}
		else if(strcmp(val->response, "real") == 0  && strcmp(val->descripition, "basic") == 0){
			comp->response = "real";
			comp->descripition = "basic";
		}
		else{
			printErrorMessageSemantic("Error: Cannot use arithmetic expressions on non basic types. Line %d", lineNumber);
			comp->response = ERROR_TYPE;
		}
		return comp;
		break;
	case TOKEN_POINTER:
		printRule("EXPRESSION_TAG -> ^");
		comp->descripition = "pointer";
		return comp;
		break;
	case TOKEN_SEPARATOR_SIGN_RB:
	case TOKEN_SEPARATOR_SIGN_RP:
	case TOKEN_SEPARATOR_SIGN_SEMICOLON:
	case TOKEN_REL_OP:
		printRule("EXPRESSION_TAG -> {EPSILON}");
		t = back_token();
		return comp;
		break;
	default:
	{
		t = back_token();
		//printErrorMessageParse(arr, 7, t);
		comp->response = ERROR_TYPE;
		return comp;
	}
	}
}

void match(eTOKENS expected){
	//Token *t = next_token();
	t = next_token();
	if (t->kind != expected)
	{
		printErrorMessageMatch(expected, t);
		t = back_token();
	}
}

void printRule(char* str){
	fprintf(f, "{%s}\n", str);
}

void printErrorMessageMatch(eTOKENS expected, Token *actual){
	fprintf(f, "Expected token '%s' at line: %d, Actual token '%s', lexeme: '%s'.\n", expected, actual->lineNumber, actual->kind, actual->lexeme);
}

void printErrorMessageParse(char** expectedKinds, int arraySize, Token *actual){
	int i;
	fprintf(f, "Expected: one of tokens ");
	for(i = 0; i < arraySize; i++)
	{
		fprintf(f, "'%s' ", expectedKinds[i]); 
	}

	fprintf(f, "at %d, Actual token: '%s', lexeme '%s'.\n", actual->lineNumber, actual->kind, actual->lexeme);
}

void printErrorMessageSemantic(char* message, int lineNumber){
	fprintf(f_semantic, message, lineNumber);
	fprintf(f_semantic, "\n");
}

compound * createCompound(){
	compound* comp = (compound*)malloc(sizeof(compound));
	comp->response = "";
	comp->descripition = "";	
	return comp;
}