#ifndef __SYNT_H
#define __SYNT_H 1

#include "semant.h"

void Prg(char *t, int *uk);
void FuncDescr(char *t, int *uk);
void VarDescr(char *t, int *uk);
void Block(char *t, int *uk);
void Operator(char *t, int *uk);
void PossibleArrInit(char *t,int *uk);
void Expression(char *t,int *uk, Node *value);
void For(char *t,int *uk);
void A1(char *t,int *uk, NodeStack *stack, Node *value);
void A2(char *t,int *uk, NodeStack *stack, Node *value);
void A3(char *t,int *uk, NodeStack *stack, Node *value);
void A4(char *t,int *uk, NodeStack *stack, Node *value);
void A5(char *t,int *uk, NodeStack *stack, Node *value);
void A6(char *t,int *uk, NodeStack *stack, Node *value);
#endif
