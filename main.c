#include "dict.h"
#include <stdio.h>
#include <string.h>

void printEach(DNode *node, Integer count, DList *list, void *ctx) {
  char typeString[25];
  char valueString[50];

  switch(node->obj->type) {
    case STRING:
      strcpy(typeString, "string");
      sprintf(valueString, "%s", node->obj->string);
      break;
    case NUMBER:
      strcpy(typeString, "number");
      sprintf(valueString, "%f", node->obj->number);
      break;
    case BOOL:
      strcpy(typeString, "boolean");
      sprintf(
        valueString,
        "%s",
        node->obj->boolean ? "True" : "False"
      );
      break;
  }

  printf("  [\x1b[1m%s\x1b[0m: %p] %s\n", node->name, node, valueString);
}

int main() {
  DList *list = AllocDList(False);

  list->setString(list, "name", "Brielle");
  list->setNumber(list, "age", 21);
  list->setNumber(list, "keysOwned", 5);
  list->setBool(list, "lovesBreakfast", True);
  list->forEach(list, printEach, NULL);

  // TODO list->write() and list->read()
  DListWrite(list, "./dict.dat");
  DListFree(list);

  list = DListRead("./dict.dat");
  list->forEach(list, printEach, NULL);
  DListFree(list);

  return 0;
}
