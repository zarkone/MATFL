#ifndef __SEMANT_H
#define __SEMANT_H 1
#include <stdio.h>

typedef enum BOOL
{
    FALSE = 0,
    TRUE = 1
} BOOL;

typedef enum DATA_TYPE
{
	dBlock = 0,
    dTInt,
    dTInt64,
    dTVoidFunc,
    dTIntMain,
    dTFor
} DATA_TYPE;

typedef struct Node
{
    DATA_TYPE type;
    union {
        int dataAsInt;
        long int dataAsInt64;
        int *dataAsIntArray;
        long int *dataAsInt64Array;
    };
    int isAssignable;
    int functionBlockUk;
	char *data, *id;
    struct Node *child, *neighbour, *parent;
	int elementsCount;

} Node;

typedef struct NodeStackItem
{
    Node *node;
    int index;

}NodeStackItem;

typedef struct NodeStack
{
    NodeStackItem **items;
    int length;
    
} NodeStack;

void init_stack(NodeStack* stack, int allocLength);
void push_stack(NodeStack* stack, Node* item, int index);
NodeStackItem* pop_stack(NodeStack* stack);

Node* addToTree(const char* lex, DATA_TYPE type, Node *current);
Node* createBlockAsChild(Node *current, Node **rollback);
void printTree(Node *node, int depth, FILE *fp);
Node* findUp(Node* current, const char* id, BOOL inBlock);

int typeCast(Node * current, Node * castee);
#endif
