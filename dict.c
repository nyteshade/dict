#include "dict.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DEBUG
int __debug_noop( const char * format, ... ) { return 0; }
int (*debug)( const char * format, ... ) = &__debug_noop;
#else
int (*debug)( const char * format, ... ) = &printf;
#endif

Object *AllocObject(void) {
  Object *object = (Object *)malloc(sizeof(Object));
  memset(object, 0L, sizeof(Object));
  return object;
}

Object *ObjectSetString(Object *object, String string) {
  memset(object, 0L, sizeof(Object));
  object->type = STRING;
  object->string = string;
  return object;
}

Object *ObjectSetNumber(Object *object, Number number) {
  memset(object, 0L, sizeof(Object));
  object->type = NUMBER;
  object->number = number;
  return object;
}

Object *ObjectSetBool(Object *object, Bool boolean) {
  memset(object, 0L, sizeof(Object));
  object->type = BOOL;
  object->boolean = boolean;
  return object;
}

void ObjectFree(Object *object) {
  debug("[DEBUG] freeing %p\n", object);
  free(object);
}

Object *DString(String string) {
  return ObjectSetString(AllocObject(), string);
}

Object *DNumber(Number number) {
  return ObjectSetNumber(AllocObject(), number);
}

Object *DBool(Bool boolean) {
  return ObjectSetBool(AllocObject(), boolean);
}


DNode *AllocDNode(void) {
  DNode *node = (DNode *)malloc(sizeof(DNode));
  memset(node, 0L, sizeof(DNode));
  return node;
}

DNode *DNodeSet(DNode *node, String name, Object *object) {
  node->name = strdup(name);
  node->obj = object;
  return node;
}

void DNodeFree(DNode *node) {
  if (node && node->name) {
    debug("[Debug] freeing %s:%p\n", node->name, node->name);
    free(node->name);
  }

  if (node) {
    debug("[Debug] freeing %p\n", node->name);
    free(node);
  }
}

DList  *AllocDList(Bool namesAreCaseSensitive) {
  DList *list = (DList *)malloc(sizeof(DList));
  memset(list, 0L, sizeof(DList));

  list->namesAreCaseSensitive = namesAreCaseSensitive;

  list->append = DListAppend;
  list->set = DListSet;
  list->setString = DListSetString;
  list->setNumber = DListSetNumber;
  list->setBool = DListSetBool;
  list->get = DListGet;
  list->getString = DListGetString;
  list->getNumber = DListGetNumber;
  list->getBool = DListGetBool;
  list->findNamed = DListFindNamed;
  list->removeNamed = DListRemoveNamed;
  list->remove = DListRemove;
  list->forEach = DListForEach;

  return list;
}

DList  *DListAppend(DList *list, DNode *node) {
  if (!list->tail) {
    list->head = list->tail = node;
    node->prev = 0L;
    node->next = 0L;
  }
  else {
    list->tail->next = node;
    node->prev = list->tail;
    node->next = 0L;
    list->tail = node;
  }

  return list;
}

DList  *DListSet(DList *list, String name, Object *value) {
  DNode *node = AllocDNode();
  DNodeSet(node, name, value);
  DListAppend(list, node);
  return list;
}

DList  *DListSetString(DList *list, String name, String value) {
  return DListSet(list, name, DString(value));
}

DList  *DListSetNumber(DList *list, String name, Number value) {
  return DListSet(list, name, DNumber(value));
}

DList  *DListSetBool(DList *list, String name, Bool value) {
  return DListSet(list, name, DBool(value));
}

Object *DListGet(DList *list, String name) {
  DNode *node = DListFindNamed(list, name);
  if (node) {
    return node->obj;
  }
  return NULL;
}

String DListGetString(DList *list, String name) {
  DNode *node = DListFindNamed(list, name);
  if (node && node->obj->type == STRING)
    return node->obj->string;
  else
    return NULL;
}

Number DListGetNumber(DList *list, String name) {
  DNode *node = DListFindNamed(list, name);
  if (node && node->obj->type == NUMBER)
    return node->obj->number;
  else
    return NULL;
}

Bool DListGetBool(DList *list, String name) {
  DNode *node = DListFindNamed(list, name);
  if (node && node->obj->type == BOOL)
    return node->obj->boolean;
  else
    return NULL;
}

DNode *DListFindNamed(DList *list, String name) {
  DNode *node = list->head;
  Object *result = NULL;
  int (*cmpFn)(const char *lhs, const char *rhs);

  cmpFn = list->namesAreCaseSensitive
    ? strcasecmp
    : strcmp;

  for (; node && node->next; node = node->next) {
    if (cmpFn(node->name, name) == 0) {
      return node;
    }
  }

  return NULL;
}

Object *DListRemoveNamed(DList *list, String name) {
  DNode *node = DListFindNamed(list, name);
  if (node) {
    return DListRemove(list, node);
  }
  else {
    return NULL;
  }
}

Object *DListRemove(DList *list, DNode *node) {
  Object *result = NULL;
  DNode *prev = node->prev, *next = node->next;

  if (prev) { prev->next = next; }
  if (next) { next->prev = prev; }
  if (list->head == node) { list->head = next; }
  if (list->tail == node) { list->tail = prev; }
  result = node->obj;
  DNodeFree(node);

  return result;
}

void DListForEach(DList *list, DListForEachFn fn) {
  DNode *node, *next;
  Integer count = 0;

  if (!list || !list->head) {
    return;
  }

  for (next = node = list->head; next; node = next) {
    next = node->next;
    fn(node, count, list);
    count++;
  }
}

void __dNodeFreeEach(DNode *node, Integer index, DList *list) {
  DListRemove(list, node);
}

void DListFree(DList *list) {
  DListForEach(list, __dNodeFreeEach);
  debug("[Debug] freeing list %p\n", list);
  free(list);
}
