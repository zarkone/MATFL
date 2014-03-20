#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <semant.h>

void printTree(Node *node, int depth, FILE *fp) {

    if (!node) { return; }
    int spaceCount = depth;

    while(spaceCount--) {
        fprintf(fp, " ");
    }
    
    fprintf(fp, "%s", node->id);

    if (node->elementsCount > 0) {
        fprintf(fp, "[%d]", node->elementsCount);

    } else if (node->type == dTIntMain || node->type == dTVoidFunc) {
        fprintf(fp, "()");
    }
    
    if (node->elementsCount > 0) {

        int i;
        
        fprintf(fp, ": [", node->dataAsInt);

        for(i=0;i<node->elementsCount;i++){
            switch(node->type){

            case dTInt: {fprintf(fp, " %d", node->dataAsIntArray[i]);}; break;
            case dTInt64: {fprintf(fp, " %d", node->dataAsInt64Array[i]);}; break;

            }
        }

        fprintf(fp, "]", node->dataAsInt);

    } else if (node->type == dTInt || node->type == dTInt64){

        fprintf(fp, ": %d", node->dataAsInt64);    
    }
    
    fprintf(fp, "\n");
        
    if (node->child) {
        printTree(node->child->neighbour, depth+1, fp);
    }
    if (node->neighbour) {
        printTree(node->neighbour, depth, fp);
    }
}
Node* addToTree(const char* lex, DATA_TYPE type, Node *current ) {
    
    Node *n = (Node *)calloc(1,sizeof(Node));
    n->type = type;
    n->id = (char *)calloc(MAXLEX,sizeof(char));
    n->data = (char *)calloc(MAXLEX,sizeof(char));
    n->elementsCount = 0;

    n->parent = current;
    
    strcpy(n->id, lex);
    current->neighbour = n;
    current = current->neighbour;

    n->isAssignable = (type == dTInt || type == dTInt64);

    return current;
}

Node* createBlockNode( Node *parent) {
    
    Node *n = (Node *)calloc(1,sizeof(Node));
    n->type = dBlock;
    n->id = (char *)calloc(8,sizeof(char));
    n->elementsCount = 0;
    strcpy(n->id,(char *) &"{ ... }\0");
    n->parent = parent;
    return n;
}
Node* createBlockAsChild(Node *current, Node ** rollback) {

    *rollback = current;

    current->child = createBlockNode(current);
    current = current->child;

    return current;

}

Node* findUp(Node* current, const char* id, BOOL inBlock) {

    if (current && current->type == dBlock && inBlock) {
        return 0;
    }
    
    if (!current || strcmp(current->id, id) == 0) {
        return current;
    }
    
    return findUp(current->parent, id, inBlock);
}

int typeCast(Node * current, Node * castee) {
    
    if (current->type == dTInt64 && castee->type == dTInt) {
        current->dataAsInt64 = castee->dataAsInt;
    } 
    else if (current->type == dTInt64 && castee->type == dTInt64) {
        current->dataAsInt64 = castee->dataAsInt64;
    } 
    else if (current->type == dTInt && castee->type == dTInt) {
        current->dataAsInt = castee->dataAsInt;
    }
    else if (current->type == dTInt && castee->type == dTInt64) {
        printf ("WARN: Cast from __int64 to int, possible loss of data. \n");
        current->dataAsInt = castee->dataAsInt64;

    } else {
        return -1;
    }
}

void init_stack(NodeStack *stack, int allocLength){

    stack->items = calloc(allocLength, sizeof(NodeStackItem*));
    stack->length = 0;
}

void push_stack(NodeStack* stack, Node* newNode, int index){

    NodeStackItem *stackItem = malloc(sizeof(NodeStackItem));

    stackItem->node = newNode;
    if (index > -1) {
        stackItem->index = index;
    } else {
        stackItem->index = -1;
    }

    stack->items[stack->length] = stackItem;
    stack->length++;
}
NodeStackItem* pop_stack(NodeStack* stack){
    
    NodeStackItem *item;
    if (stack->length == 0) {
        item = 0;
    } else {
        item = malloc(sizeof(NodeStackItem));
        item->node = stack->items[stack->length-1]->node;
        item->index = stack->items[stack->length-1]->index;
        stack->length--;
    }

    return item;
    

}
