#ifndef chad_dictionary_h
#define chad_dictionary_h

#include "common.h"
#include "value.h"

typedef struct {
  ObjString* key;
  Value value;
} Entry;

typedef struct {
  int count;
  int capacity;
  Entry* entries;
} Dictionary;

void initDict(Dictionary* dict);
void freeDict(Dictionary* dict);
bool dictSet(Dictionary* dict, ObjString* key, Value value);
bool tableDelete(Dictionary* dict, ObjString* key);
bool dictGet(Dictionary* dict, ObjString* key, Value* value);
void dictAddAll(Dictionary* from, Dictionary* to);
ObjString* dictFindString(Dictionary* dict, const char* chars,
                           int length, uint32_t hash);

#endif
