#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "obj.h"
#include "dictionary.h"
#include "value.h"

#define DICTIONARY_MAX_LOAD 0.75

void initDict(Dictionary* dict) {
  dict->count = 0;
  dict->capacity = 0;
  dict->entries = NULL;
}

void freeDict(Dictionary* dict) {
  FREE_ARRAY(Entry, dict->entries, dict->capacity);
  initDict(dict);
}

static Entry* findEntry(Entry* entries, int capacity, ObjString* key)
{
  uint32_t index = key->hash % capacity;
  Entry* tombstone = NULL;
  for (;;) {
    Entry* entry = &entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        return tombstone != NULL ? tombstone : entry;
      } else {
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (entry->key == key) {
      return entry;
    }
    index = (index + 1) % capacity;
  }
}

bool dictCheck(Dictionary* dict, ObjString* key) {
  if (dict->count == 0) return false;
  Entry* entry = findEntry(dict->entries, dict->capacity, key);
  if (entry->key == NULL) return false;
  return true;
}

bool dictGet(Dictionary* dict, ObjString* key, Value* value) {
  if (dict->count == 0) return false;

  Entry* entry = findEntry(dict->entries, dict->capacity, key);
  if (entry->key == NULL) return false;

  *value = entry->value;
  return true;
}

static void adjustCapacity(Dictionary* dict, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NIL_VAL;
  }

  dict->count = 0;
  for (int i = 0; i < dict->capacity; i++) {
    Entry* entry = &dict->entries[i];
    if (entry->key == NULL) continue;

    Entry* dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    dict->count++;
  }

  dict->entries = entries;
  dict->capacity = capacity;
}

bool dictAdd(Dictionary* dict, ObjString* key) {
  if (dict->count + 1 > dict->capacity * DICTIONARY_MAX_LOAD) {
    int capacity = GROW_CAPACITY(dict->capacity);
    adjustCapacity(dict, capacity);
  }
  Entry* entry = findEntry(dict->entries, dict->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && IS_NIL(entry->value)) dict->count++;

  entry->key = key;
  return isNewKey;
}

bool dictSet(Dictionary* dict, ObjString* key, Value value) {

  if (dict->count + 1 > dict->capacity * DICTIONARY_MAX_LOAD) {
    int capacity = GROW_CAPACITY(dict->capacity);
    adjustCapacity(dict, capacity);
  }
  Entry* entry = findEntry(dict->entries, dict->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && IS_NIL(entry->value)) dict->count++;

  entry->key = key;
  entry->value = value;
  return isNewKey;
}

bool dictDelete(Dictionary* dict, ObjString* key) {
  if (dict->count == 0) return false;

  Entry* entry = findEntry(dict->entries, dict->capacity, key);
  if (entry->key == NULL) return false;

  entry->key = NULL;
  entry->value = BOOL_VAL(true);
  return true;
}

void dictAddAll(Dictionary* from, Dictionary* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
    if (entry->key != NULL) {
      dictSet(to, entry->key, entry->value);
    }
  }
}

ObjString* dictFindString(Dictionary* dict, const char* chars,
                           int length, uint32_t hash) {
  if (dict->count == 0) return NULL;

  uint32_t index = hash % dict->capacity;
  for (;;) {
    Entry* entry = &dict->entries[index];
    if (entry->key == NULL) {
      // Stop if we find an empty non-tombstone entry.
      if (IS_NIL(entry->value)) return NULL;
    } else if (entry->key->length == length &&
        entry->key->hash == hash &&
        memcmp(entry->key->chars, chars, length) == 0) {
      // We found it.
      return entry->key;
    }

    index = (index + 1) % dict->capacity;
  }
}
