#ifndef HASHTABLE_H
#define HASHTABLE_H

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
}ElementAttribute;

typedef struct entry_s {
	char* key;
	ElementAttribute* value;
	struct entry_s *next;
}entry_s;

typedef struct hashtable_s {
	int size;
	struct entry_s **table;
}hashtable_s;

ElementAttribute* createElementAttribute(); //creates and returns a default valued element attribute
hashtable_s *ht_create(int size); //Create a new hashtable.
int ht_hash(hashtable_s *hashtable, char *key); //Hash a new string for a particular hash table.
entry_s *ht_newpair(char *key, ElementAttribute* value); //Create a key-value pair.
void ht_insert(hashtable_s *hashtable, char *key, ElementAttribute* value); // Insert a key-value pair into a hash table.
ElementAttribute *ht_find(hashtable_s *hashtable, char *key); // Find a key-value pair if key exists in a hash table.

#endif