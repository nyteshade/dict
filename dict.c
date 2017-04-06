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

void ObjectTypeToString(ObjectType type, String buffer) {
  switch (type) {
    case STRING:
      strcpy(buffer, "String");
      return;
    case NUMBER:
      strcpy(buffer, "Number");
      return;
    case BOOL:
      strcpy(buffer, "Boolean");
      return;
  };
}

ObjectType StringToObjectType(String typeString) {
  if (
    strcasecmp(typeString, "str") == 0 ||
    strcasecmp(typeString, "string") == 0
  ) {
    return STRING;
  }

  if (
    strcasecmp(typeString, "num") == 0 ||
    strcasecmp(typeString, "number") == 0 ||
    strcasecmp(typeString, "no") == 0
  ) {
    return NUMBER;
  }

  if (
    strcasecmp(typeString, "bool") == 0 ||
    strcasecmp(typeString, "boolean") == 0
  ) {
    return BOOL;
  }

  return 0;
}

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

void DListForEach(DList *list, DListForEachFn fn, void *context) {
  DNode *node, *next;
  Integer count = 0;

  if (!list || !list->head) {
    return;
  }

  for (next = node = list->head; next; node = next) {
    next = node->next;
    fn(node, count, list, context);
    count++;
  }
}

void __dNodeFreeEach(DNode *node, Integer index, DList *list, void *ctx) {
  DListRemove(list, node);
}

void DListFree(DList *list) {
  DListForEach(list, __dNodeFreeEach, NULL);
  debug("[Debug] freeing list %p\n", list);
  free(list);
}

Bool __writeString(FILE *file, String string) {
  Integer length = (Integer)strlen(string);
  ssize_t xfer = 0;
  Bool error = False;

  error = (!error && (xfer = fwrite(
    &length,
    sizeof(Integer),
    1,
    file
  )) < 0);

  error = (!error && (xfer = fwrite(
    string,
    sizeof(char) * length,
    1,
    file
  )) < 0);
  return error;
}

String __readString(FILE *file) {
  Integer length;
  ssize_t xfer;
  String result;

  debug("[__readString] %p\n", file);

  xfer = fread(&length, sizeof(Integer), 1, file);
  debug("[__readString] read %d bytes\n", (long)xfer);
  if (xfer <= 0) return NULL;

  debug("[__readString] allocating %d bytes...", length + 1);
  result = (String)malloc(sizeof(char) * (length + 1));
  debug("%s\n", result ? "done" : "error");
  if (!result) return NULL;

  debug("[__readString] clearing %d bytes...", (length + 1) * sizeof(char));
  memset(result, 0L, sizeof(char) * (length + 1));
  debug("done\n");
  xfer = 0;
  xfer = fread(result, sizeof(char) * length, 1, file);
  debug("[__readString] read %d bytes\n", xfer);
  if (xfer <= 0) {
    debug("[__readString] ERROR!\n");
    free(result);
    return NULL;
  }

  debug("[__readString] read %s\n", result);
  return result;
}

Bool __writeInteger(FILE *file, Integer integer) {
  ssize_t xfer = fwrite(&integer, sizeof(Integer), 1, file);
  return xfer > 0;
}

Integer __readInteger(FILE *file) {
  Integer result;
  fread(&result, sizeof(Integer), 1, file);
  return result;
}

Bool __writeNumber(FILE *file, Number number) {
  ssize_t xfer = fwrite(&number, sizeof(Number), 1, file);
  return xfer > 0;
}

Number __readNumber(FILE *file) {
  Number result;
  fread(&result, sizeof(Number), 1, file);
  return result;
}

Bool __writeBool(FILE *file, Bool boolean) {
  ssize_t xfer = fwrite(&boolean, sizeof(Bool), 1, file);
  return xfer > 0;
}

Bool __readBool(FILE *file) {
  Bool result;
  fread(&result, sizeof(Bool), 1, file);
  return result;
}

Bool __writeType(FILE *file, ObjectType type) {
  ssize_t xfer = fwrite(&type, sizeof(ObjectType), 1, file);
  return xfer > 0;
}

Number __readType(FILE *file) {
  ObjectType result;
  ssize_t xfer;
  xfer = fread(&result, sizeof(ObjectType), 1, file);
  return xfer <= 0 ? -1 : result;
}

void __dNodeWriteEach(DNode *node, Integer index, DList *list, void *ctx) {
  FILE *file = (FILE *)ctx;
  __writeType(file, node->obj->type);
  __writeString(file, node->name);
  switch (node->obj->type) {
    case STRING:
      __writeString(file, node->obj->string);
      break;
    case NUMBER:
      __writeNumber(file, node->obj->number);
      break;
    case BOOL:
      __writeBool(file, node->obj->boolean);
      break;
  }
}

void DListWriteFP(DList *list, FILE *file) {
  list->forEach(list, __dNodeWriteEach, file);
}

void DListWrite(DList *list, String fileName) {
  FILE *file = fopen((const char *)fileName, "w+");
  if (file) {
    DListWriteFP(list, file);
  }
  fclose(file);
}

DList  *DListReadFP(FILE *file) {
  ObjectType type;
  ObjectData data;
  Integer length;
  String key;
  Bool error = False;
  DList *list = AllocDList(False);
  Bool done = False;
  char typeString[20];

  while (!done) {
    debug("...reading type: ");
    type = __readType(file);
    if (!type) { debug("failed!\n"); done = True; break; }
    ObjectTypeToString(type, typeString);
    debug("%s\n", typeString);

    debug("...reading key : ");
    key = __readString(file);
    if (key) {
      debug("%s\n", key);
      switch (type) {
        case STRING:
          debug("...reading str : ");
          data.string = __readString(file);
          debug("%s\n", data.string);
          list->setString(list, key, data.string);
          free(key);
          continue;
        case NUMBER:
          debug("...reading num : ");
          data.number = __readNumber(file);
          debug("%f\n", data.number);
          list->setNumber(list, key, data.number);
          free(key);
          continue;
        case BOOL:
          debug("...reading bool: ");
          data.boolean = __readBool(file);
          debug("%s\n", data.boolean ? "True" : "False");
          list->setBool(list, key, data.boolean);
          free(key);
          continue;
      }
    }
  }

  return list;
}

DList  *DListRead(String fileName) {
  debug("Reading list (%s)...", fileName);
  FILE *file = fopen(fileName, "r+");
  debug("%s\n", file ? "True" : "False");
  DList *list = DListReadFP(file);
  fclose(file);

  return list;
}
