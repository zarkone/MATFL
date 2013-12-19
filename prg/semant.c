#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <semant.h>

/**
 * IDEA: to allocate memory for Node*s by pages.
 * Page size could be set by command-line param, for example.
 * It should decrease time of parsing by redusing number of sys calls.
 *
 */
void printTree(Node *node, int depth, FILE *fp) {
    if (!node) { return; }

    int spaceCount = depth;

    while(spaceCount--) {
        fprintf(fp, " ");
    }
    
    fprintf(fp, "%s\n", node->id);

    if (node->child) {
        printTree(node->child, depth+1, fp);
    }
    if (node->neighbour) {
        printTree(node->neighbour, depth, fp);
    }
}
Node* addToTree(const char* lex, DATA_TYPE type, Node *current, Node **rollback) {
    
    printf("add %s to %s", lex, current->id);
    
    if (current->type == dBlock && current->parent) {
        printf(" <-- %s", current->parent->id);
    } 
    printf("\n");
    
    Node *n = (Node *)calloc(1,sizeof(Node));
    n->type = type;
    n->id = (char *)calloc(MAXLEX,sizeof(char));
    n->data = (char *)calloc(MAXLEX,sizeof(char));
    n->elementsCount = 0;
    // n->parent = current->parent ? current->parent : current;
    n->parent = current;
    
    strcpy(n->id, lex);
    current->neighbour = n;
    current = current->neighbour;
        
	if (type == dTInt || type == dTInt64) {

		n->isAssignable = TRUE;
		return current;
		
	} else {
		
		n->isAssignable = FALSE;
		Node *vertex = current;

		current->child = (Node *)calloc(1,sizeof(Node));
        current->child->parent = current;
		current = current->child;
		current->type = dBlock;
		current->id = (char *)calloc(8,sizeof(char));
		current->elementsCount = 0;
		strcpy(current->id,(char *) &"{ ... }\0");

        *rollback = vertex;
		return current;
	}
}
