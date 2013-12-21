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
        fprintf(fp, "()", node->elementsCount);
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


