#ifndef __SYNT_H
#define __SYNT_H 1

#include "semant.h"

void Prg(char *t, int *uk);
void FuncDescr(char *t, int *uk);
void VarDescr(char *t, int *uk);
void Block(char *t, int *uk);
void Operator(char *t, int *uk);
void PossibleArrInit(char *t,int *uk);
Node * Expression(char *t,int *uk);
void For(char *t,int *uk);
Node * A1(char *t,int *uk, NodeStack *stack);
Node * A2(char *t,int *uk, NodeStack *stack);
Node * A3(char *t,int *uk, NodeStack *stack);
Node * A4(char *t,int *uk, NodeStack *stack);
Node * A5(char *t,int *uk, NodeStack *stack);
Node * A6(char *t,int *uk, NodeStack *stack);
#endif
