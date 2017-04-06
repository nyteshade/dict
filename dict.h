#ifndef DICT_H
#define DICT_H

#ifndef NULL
#define NULL 0L
#endif

#include <stdio.h>

typedef long Integer;
typedef float Number;
typedef char *String;

typedef enum {
  False = 0,
  True = 1
} Bool;

typedef union {
  String string;
  Number number;
  Bool boolean;
} ObjectData;

typedef enum {
  STRING = 1,
  NUMBER = 2,
  BOOL = 4
} ObjectType;

typedef struct Object {
  ObjectType type;
  ObjectData;
} Object;

typedef struct DNode {
  struct DNode *next;
  struct DNode *prev;
  String name;
  Object *obj;
} DNode;

struct DList;
typedef void (*DListForEachFn)(
  DNode *node,
  Integer index,
  struct DList *list,
  void *context
);

typedef struct DList {
  DNode *head;
  DNode *tail;

  Bool namesAreCaseSensitive;

  struct DList  *(*append)(struct DList *list, DNode *node);
  struct DList  *(*set)(struct DList *list, String name, Object *value);
  struct DList  *(*setString)(struct DList *list, String name, String value);
  struct DList  *(*setNumber)(struct DList *list, String name, Number value);
  struct DList  *(*setBool)(struct DList *list, String name, Bool value);
  Object        *(*get)(struct DList *list, String name);
  String         (*getString)(struct DList *list, String name);
  Number         (*getNumber)(struct DList *list, String name);
  Bool           (*getBool)(struct DList *list, String name);
  DNode         *(*findNamed)(struct DList *list, String name);
  Object        *(*removeNamed)(struct DList *list, String name);
  Object        *(*remove)(struct DList *list, DNode *node);
  void           (*forEach)(struct DList *list, DListForEachFn fn, void *ctx);
} DList;

void       ObjectTypeToString(ObjectType type, String buffer);
ObjectType StringToObjectType(String typeString);

Object *AllocObject(void);
Object *ObjectSetString(Object *object, String string);
Object *ObjectSetNumber(Object *object, Number number);
Object *ObjectSetBool(Object *object, Bool boolean);
void    ObjectFree(Object *object);

Object *DString(String string);
Object *DNumber(Number number);
Object *DBool(Bool boolean);

DNode *AllocDNode(void);
DNode *DNodeSet(DNode *node, String name, Object *object);
void   DNodeFree(DNode *node);

DList  *AllocDList(Bool namesAreCaseSensitive);
DList  *DListAppend(DList *list, DNode *node);
DList  *DListSet(DList *list, String name, Object *value);
DList  *DListSetString(DList *list, String name, String value);
DList  *DListSetNumber(DList *list, String name, Number value);
DList  *DListSetBool(DList *list, String name, Bool value);
Object *DListGet(DList *list, String name);
String  DListGetString(DList *list, String name);
Number  DListGetNumber(DList *list, String name);
Bool    DListGetBool(DList *list, String name);
DNode  *DListFindNamed(DList *list, String name);
Object *DListRemoveNamed(DList *list, String name);
Object *DListRemove(DList *list, DNode *node);
void    DListForEach(DList *list, DListForEachFn fn, void *context);
void    DListFree(DList *list);
void    DListWriteFP(DList *list, FILE *file);
void    DListWrite(DList *list, String fileName);
DList  *DListReadFP(FILE *file);
DList  *DListRead(String fileName);

#endif
