///*#include "HashTable.h"
//
//ElementAttribute* createElementAttribute(){
//	ElementAttribute* elemAttr = (ElementAttribute*)malloc(sizeof(ElementAttribute));
//	elemAttr->name = "";
//	elemAttr->role = "";
//	elemAttr->type = "";
//	elemAttr->category = "";
//	elemAttr->subtype = "";
//	elemAttr->size = 0;
//	return elemAttr;
//}
//
//hashtable_s *ht_create(int size){
//	hashtable_s *hashtable = NULL;
//	int i;
//
//	if (size<1)
//		return NULL;
//
//	/* Allocate the table itself. */
//	if ((hashtable = (hashtable_s*)malloc(sizeof(hashtable_s))) == NULL){
//		return NULL;
//	}
//
//	/* Allocate pointers to the head nodes. */
//	if ((hashtable->table = (entry_s**)malloc(sizeof(entry_s*) * size)) ==NULL){
//		return NULL;
//	}
//
//	for (i=0; i<size; i++){
//		hashtable->table[i] = NULL;
//	}
//
//	hashtable->size = size;
//
//	return hashtable;
//}
//
//int ht_hash(hashtable_s *hashtable, char *key){
//	unsigned long int hashval = 0;
//	int i=0;
//
//	/* Convert our string to an integer */
//	while (hashval < ULONG_MAX && i < strlen(key)){
//		hashval = hashval << 8;
//		hashval += key[i];
//		i++;
//	}
//
//	return hashval % hashtable->size;
//}
//
//entry_s *ht_newpair(char *key, ElementAttribute* value){
//	entry_s *newpair;
//
//	if((newpair = (entry_s*)malloc(sizeof(entry_s))) == NULL){
//		return NULL;
//	}
//
//	newpair->next = NULL;
//
//	return newpair;
//}
//
//void ht_insert(hashtable_s *hashtable, char* key, ElementAttribute* value){
//	int bin = 0;
//	entry_s *newpair = NULL;
//	entry_s *next = NULL;
//	entry_s *last = NULL;
//
//	bin = ht_hash(hashtable, key);
//
//	next = hashtable->table[bin];
//
//	while(next!= NULL && next->key != NULL && strcmp(key, next->key) > 0){
//		last = next;
//		next = next->next;
//	}
//
//	newpair = ht_newpair(key, value);
//
//	//Start of linked list in that bin
//	if(next == hashtable->table[bin]){
//		newpair->next = next;
//		hashtable->table[bin] = newpair;
//	}
//	//End of linked list in that bin
//	else if (next == NULL){
//		last->next = newpair;
//	}
//	//Middle of linked list in that bin
//	else {
//		newpair->next = next;
//		last->next = newpair;
//	}
//}
//
//ElementAttribute *ht_find(hashtable_s *hashtable, char *key){
//	int bin = 0;
//	entry_s *pair;
//
//	bin = ht_hash(hashtable, key);
//
//	pair = hashtable->table[bin];
//	while(pair != NULL && pair->key != NULL && strcmp(key, pair->key) > 0){
//		pair = pair->next;
//	}
//
//	if(pair == NULL || pair->key == NULL || strcmp(key, pair->key) != 0){
//		return NULL;
//	}
//	else {
//		return pair->value;
//	}
//}