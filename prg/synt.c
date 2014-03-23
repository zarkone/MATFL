
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <scaner.h>
#include <synt.h>
#include <semant.h>
#include <limits.h>

Node *current;
int INTERPRET_FLAG = 1;

void Prg(char *t, int *uk){

    // lex=(char *)calloc(MAXLEX,sizeof(char));	
    int oldUk = *uk, T = scan(lex, t, uk);

    Node *root = (Node *)calloc(1, sizeof(Node));
    root->type = dBlock;
    root->isAssignable = 0;
    root->parent = 0;
    root->elementsCount = 0;
    root->id = (char *)calloc(5, sizeof(char));
    strcpy(root->id,(char *) &"^^^^\0");

    current = root;
            
	while(T == TInt || T == TInt64 || T == TVoid || T == TEnd) {
        
        if (T == TEnd) {

            FILE *fp = fopen("semant-tree.txt", "w");

            printTree(root->neighbour, 0, fp);
            fclose(fp);
            
            printf ("\n -*- Exit Success -*-\n\n");
            exit(EXIT_SUCCESS);
        }
        
		if (T == TVoid) { *uk = oldUk; FuncDescr(t, uk); }
		else if (T == TInt64) { *uk = oldUk; VarDescr(t, uk); }
		else if (T == TInt) {

			T = scan(lex, t, uk); 
			if (T == TMain) { *uk = oldUk; FuncDescr(t, uk);  }
			else if (T == Tid) { *uk = oldUk; VarDescr(t, uk); }
			else {
                printf ("Expected `<id>` or `<main>`, got: %x (uk: %d, lex: %s)", T, *uk, lex);
                exit(EXIT_FAILURE);
            }

		} else {
            printf ("Expected `<function descr>` or `<var descr>`, got: %x (uk: %d)", T, *uk, lex);
            exit(EXIT_FAILURE);
        }

        oldUk = *uk;
        T = scan(lex,t,uk);
        
	}

    
    printf ("Expected EOF, <void> or <int >, got: uk: %d, lex: %s, T: %x", *uk, lex, T);
    exit(EXIT_FAILURE);

}

void FuncDescr(char *t, int *uk){

	int	oldUk, type,
		T = scan(lex, t, uk),
		T1 = scan(lex, t, uk);
	
	if (!((T == TVoid  && T1 == Tid) || (T == TInt && T1 == TMain))) {
        printf("Expected `void <id>` or `int main`, got: %x (uk: %d, lex: %s)", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    type = (T1 == TMain) ? dTIntMain : dTVoidFunc;

    if ( !findUp(current, lex, FALSE) ) {
        current = addToTree(lex, type, current);
        
    } else {
        printf ("ERROR: Function %s is already declared. \n", lex);
        exit(EXIT_FAILURE);
    }

    
    T = scan(lex, t, uk);
    T1 = scan(lex, t, uk);

    if (!(T == TCrBrackOpen && T1 == TCrBrackClose)) {
        printf ("Expected `()`, got: %x (uk: %d, lex: %s)", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    oldUk = *uk;
    T = scan(lex, t, uk);

    *uk = oldUk;

    if (!(T == TEgBrackOpen)) {
        printf ("Expected `{ `, got: %x (uk: %d, lex: %s)", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    current->functionBlockUk = *uk;

    if (type != dTIntMain) { INTERPRET_FLAG = 0; }
    Block(t, uk);
    if (type != dTIntMain) { INTERPRET_FLAG = 1; }
    


}

void VarDescr(char *t, int *uk){

	int	oldUk, type,
		T = scan(lex, t, uk);

    if (!(T == TInt || T == TInt64)) {
        printf ("Expected `int` or `__int64` , got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    type = (T == TInt) ? dTInt : dTInt64;
    
    do {

        T = scan(lex, t, uk);
        
        if (T != Tid) {
            printf ("Expected `<id>`, got: %x, uk: %d, lex: %s", T, *uk, lex);
            exit(EXIT_FAILURE);
        }

        char *id = (char *)calloc(strlen(lex), sizeof(char));
        strcpy(id, lex);
        int idUk = *uk;

        Node *declaredVar = findUp(current, id, TRUE);
        
        if ( declaredVar ) {
            printf ("ERROR: Variable %s is declared before. [id: %d] \n", id, idUk);
            exit(EXIT_FAILURE);
        }
        
        current = addToTree(lex, type, current);
        
        T = scan(lex, t, uk);
        
        if (T == TSqBrackOpen) {

            T = scan(lex, t, uk);
            if (!(T == TConstDec || T == TConstHex)) {
                printf ("Expected `DecConst` or `HexConst`, got: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            }

            int base = (T == TConstDec) ? 10 : 16;
            current->elementsCount = strtol(lex,0,base);
           
            if (current->elementsCount < 1) {
                printf ("Array length must me at least 1: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            }

            T = scan(lex, t, uk);

            if (T != TSqBrackClose) {
                printf ("Expected `]`, got: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            }
                
            if (INTERPRET_FLAG) {
                switch(current->type) {

                case dTInt: { current->dataAsIntArray = (int *)calloc(current->elementsCount, sizeof(int)); };break;
                case dTInt64: { current->dataAsInt64Array = (long int *)calloc(current->elementsCount, sizeof(long int)); };break;

                }
            }

            PossibleArrInit(t,uk);
            T = scan(lex, t, uk);
            
        } else if (T == TAssign) {

            Node * value = malloc(sizeof(Node));        
            Expression(t,uk, value);
            
            if (INTERPRET_FLAG) {

                if (typeCast(current, value) < 0) {

                    printf ("Expected int and __int64 types to cast, got: %d, %d\n", current->type, value->type);
                    printf (" uk: %d, lex: %s",  *uk, lex);
                    printf ("%s %d\n",value->id, value->dataAsInt64);
                    exit(EXIT_FAILURE);
                }

            }
            free(value);

            T = scan(lex, t, uk);
        }
        
        
        
    } while(T == TSem);

    if (T != TComma) {
        printf ("Expected `;`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }
    
}

void Block(char *t, int *uk){

    int oldUk = *uk, T = scan(lex,t,uk);
    
    if (!(T == TEgBrackOpen)) {
        printf ("Expected `{ `, got: %x (uk: %d, lex: %s)", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    Node *rollback;
    current = createBlockAsChild(current, &rollback);
    
    while(T != TEgBrackClose) {

        oldUk = *uk;
        T = scan(lex,t,uk);
        
        if (T == TInt || T == TInt64) {

            *uk = oldUk;
            VarDescr(t,uk);
            
        } else if (T == TCrBrackOpen ||
                   T == Tid ||
                   T == TComma ||
                   T == TConstDec ||
                   T == TConstHex ||
                   T == TFor ||
                   T == TEgBrackOpen) {
            

            *uk = oldUk;

            Operator(t,uk);

        } else if (T != TEgBrackClose) {
            printf ("Expected `<var descr>` or `<operator>`, got: %x (uk: %d, lex: %s)", T, *uk, lex);
            exit(EXIT_FAILURE);

        }
    }
    
    current = rollback;

}
void Operator(char *t,int *uk){

    int oldUk = *uk, T = scan(lex,t,uk);
    Node * value = malloc(sizeof(Node));        

    if (T == TFor) {

        *uk = oldUk;
        For(t,uk);
        return;
        
    } else if (T == TEgBrackOpen) {

        *uk = oldUk;

        current = current->neighbour = createBlockNode(current);

        // current = addToTree(&"{ ... }", dBlock, current);
        Block(t,uk);
        return;
        
    }
    
    if (T == Tid) {

        char *id = (char *)calloc(strlen(lex), sizeof(char));
        strcpy(id, lex);
        int idUk = *uk;
        
        T = scan(lex,t,uk);
        
        if (T == TCrBrackOpen) {

            T = scan(lex,t,uk);

            if (T != TCrBrackClose) {
                printf ("Expected `<id>()`, got: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            } 
                
            Node *declaredFunction = findUp(current, id, FALSE);
                
            if ( !declaredFunction ) {
                printf ("ERROR: Function %s is not declared. [uk: %d] \n", id, idUk);
                exit(EXIT_FAILURE);
            }

            BOOL isFunction = (declaredFunction->type == dTVoidFunc);
            if (!isFunction) {
                printf ("ERROR:  %s is declared but it is not a function. [uk: %d] \n", id, idUk);
                exit(EXIT_FAILURE);
            }

            Node *oldNeighbour, *oldCurrent;

            if (declaredFunction->neighbour) {
                oldNeighbour = declaredFunction->neighbour;
            }
            oldCurrent = current;

            current = declaredFunction->neighbour = createBlockNode(current);
            current->neighbour = oldNeighbour;
            
            int beforeCallUk = *uk;

            *uk = declaredFunction->functionBlockUk;

            printf("befor: %d \n", *uk);
            Block(t,uk);

            *uk = beforeCallUk;
            current = oldCurrent;

            T = scan(lex,t,uk);

        } else {
            
            *uk = oldUk;
            Expression(t,uk,value);
            T = scan(lex,t,uk);
        }
        
    } else if (T == TCrBrackOpen ||
               T == TConstDec ||
               T == TConstHex ) {

        *uk = oldUk;
        Expression(t,uk, value);
        T = scan(lex,t,uk);
     
    }
    
    if (T != TComma) {
        printf ("Expected  `;`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    free(value);
    
}

void PossibleArrInit(char *t,int *uk) {

    int oldUk = *uk, 
        T = scan(lex,t,uk);

    if (T != TAssign) { *uk = oldUk; return; }

    T = scan(lex,t,uk);
    if (T != TEgBrackOpen) {
        printf ("Expected  `{`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }
    
    int eIndex = current->elementsCount;

    do {
        
        if (eIndex < 1) {
            printf ("To few elements in array init. uk: %d ", *uk);
            exit(EXIT_FAILURE);
        }

        T = scan(lex,t,uk);

        if (T != TConstDec && T != TConstHex) {
            printf ("Expected  `ConstDec` or `ConstHex`, got: %x, uk: %d, lex: %s", T, *uk, lex);
            exit(EXIT_FAILURE);
        }

        if (INTERPRET_FLAG) {
            int base = (T == TConstDec) ? 10 : 16;

            switch(current->type)
            {
            case dTInt: { current->dataAsIntArray[current->elementsCount - eIndex] = strtol(lex,0,base); }; break;
            case dTInt64: { current->dataAsInt64Array[current->elementsCount - eIndex] = strtol(lex,0,base); }; break;
            }
        }

        T = scan(lex,t,uk);
        if (T == TEgBrackClose) { break; }

        eIndex--;

    } while(T == TSem);
    
    if (T != TEgBrackClose) {
        printf ("Expected  `}`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

}

void Expression(char *t,int *uk, Node *value){

    int oldUk = *uk, T;
    NodeStack *stack;

    if (INTERPRET_FLAG) {
        stack = malloc(sizeof(NodeStack));
        init_stack(stack, 128); 
    }

    do {
        
        A2(t,uk, stack, value);
        oldUk = *uk;
        T = scan(lex,t,uk);
        
        if ( INTERPRET_FLAG && T == TAssign && !value->isAssignable) {
            printf ("lvalue required.  uk: %d, lex: %s",  *uk, lex);
            exit(EXIT_FAILURE);
        }

    } while(T == TAssign);

    if (INTERPRET_FLAG && stack->length > 0) {
        NodeStackItem *var;
        
        while(var = pop_stack(stack)) {

            if (var->index > -1) {

                switch(var->node->type) {

                case dTInt: { var->node->dataAsIntArray[var->index] = value->dataAsInt;  }; break;
                case dTInt64: { var->node->dataAsInt64Array[var->index] = value->dataAsInt64;  }; break;

                }
            }
            else {
                typeCast(var->node, value);
            }

            value->type = var->node->type;
            value->dataAsInt64 = var->node->dataAsInt64;

            free(var);
        }

    }

    if (INTERPRET_FLAG) {
        free(stack->items);
    }

    *uk = oldUk;
    
}

void A2(char *t,int *uk,NodeStack *stack,  Node *value){

    int oldUk = *uk;
    int stackLengthBefore = stack ? stack->length : 0;

    A3(t,uk,stack, value);

    int stackLengthAfter = stack ? stack->length : 0;

    oldUk = *uk;
    int T = scan(lex,t,uk);
    Node *newValue;

    if (INTERPRET_FLAG) {
        newValue = malloc(sizeof(Node));
    }

    while (T == TAnd || T == TOr) {

        if (stackLengthAfter != stackLengthBefore) {
            pop_stack(stack);
        }

        A3(t,uk,0,newValue);

        if (INTERPRET_FLAG) {
            value->isAssignable = 0;
            if (T == TAnd) {
                value->dataAsInt64 = value->dataAsInt64 && newValue->dataAsInt64;
            } else {
                value->dataAsInt64 = value->dataAsInt64 || newValue->dataAsInt64;
            }
        }

        oldUk = *uk;
        T = scan(lex,t,uk);
    }

    if (INTERPRET_FLAG) {
        free(newValue);
    }

    *uk = oldUk;
}
void A3(char *t,int *uk,NodeStack *stack,  Node *value){

    int oldUk = *uk, isNot = 0;

    int T = scan(lex,t,uk);
    
    if (T != TNot) { *uk = oldUk; }
    else { isNot = 1; }

    int stackLengthBefore = stack ? stack->length : 0;    

    A4(t,uk,stack,value);

    int stackLengthAfter = stack ? stack->length : 0;

    if (INTERPRET_FLAG && isNot) { 

        if (stackLengthAfter != stackLengthBefore) {
            pop_stack(stack);
        }

        value->isAssignable = 0;
        value->dataAsInt64 = !value->dataAsInt64; 
    }  

    oldUk = *uk;
    T = scan(lex,t,uk);

    Node *newValue;

    if (INTERPRET_FLAG) {
        newValue= malloc(sizeof(Node));
    }

    while (T == TGrEq || T == TGr || T == TLess || T == TLessEq || T == TEq || T == TNotEq) {

        if (stackLengthAfter != stackLengthBefore) {
            pop_stack(stack);
        }

        A4(t,uk,0,newValue);

        if (INTERPRET_FLAG) {
            switch(T) {
   
            case TGrEq: { value->dataAsInt64 = value->dataAsInt64 >= newValue->dataAsInt64; };break;
            case TGr: { value->dataAsInt64 = value->dataAsInt64 > newValue->dataAsInt64; };break;
            case TLess: { value->dataAsInt64 = value->dataAsInt64 < newValue->dataAsInt64; };break;
            case TLessEq: { value->dataAsInt64 = value->dataAsInt64 <= newValue->dataAsInt64; };break;
            case TEq: { value->dataAsInt64 = value->dataAsInt64 == newValue->dataAsInt64; };break;
            case TNotEq: { value->dataAsInt64 = value->dataAsInt64 != newValue->dataAsInt64; };break;

            }
        }

        oldUk = *uk;
        T = scan(lex,t,uk);

    }

    if (INTERPRET_FLAG) {
        free(newValue);    
    }

    *uk = oldUk;
}

void A4(char *t,int *uk,NodeStack *stack, Node *value){

    int oldUk = *uk, isNegative = 0;

    int T = scan(lex,t,uk);
    if (T != TPlus && T != TMinus) { *uk = oldUk; }
    else if (T == TMinus) { isNegative = 1; }

    int stackLengthBefore = stack ? stack->length : 0;

    A5(t,uk,stack, value);

    int stackLengthAfter = stack ? stack->length : 0;

    if (INTERPRET_FLAG && isNegative) { 

        if (stackLengthAfter != stackLengthBefore) {
            pop_stack(stack);
        }

        value->isAssignable = 0;
        value->dataAsInt64 = -value->dataAsInt64; 
    }

    oldUk = *uk;
    T = scan(lex,t,uk);
    Node *newValue;

    if (INTERPRET_FLAG) {
        newValue = malloc(sizeof(Node));
    }

    while (T == TPlus || T == TMinus ) {

        if (stackLengthAfter != stackLengthBefore) {
            pop_stack(stack);
        }

        A5(t,uk,0, newValue);

        if (INTERPRET_FLAG) {        
            value->isAssignable = 0;

            switch(T) {
   
            case TPlus: { value->dataAsInt64 = value->dataAsInt64 + newValue->dataAsInt64; };break;
            case TMinus: { value->dataAsInt64 = value->dataAsInt64 - newValue->dataAsInt64; };break;

            }
        }

        oldUk = *uk;
        T = scan(lex,t,uk);

    }

    if (INTERPRET_FLAG) {        
        free(newValue);    
    }

    *uk = oldUk;

}

void A5(char *t,int *uk,NodeStack *stack,  Node *value){

    int oldUk = *uk;

    int stackLengthBefore = stack ? stack->length : 0;

    A6(t,uk,stack, value);

    int stackLengthAfter = stack ? stack->length : 0;

    oldUk = *uk;
    int T = scan(lex,t,uk);
    Node *newValue;

    if (INTERPRET_FLAG) {        
        newValue = malloc(sizeof(Node));
    }

    while (T == TMul || T == TDiv || T == TRest ) {

        if (stackLengthAfter != stackLengthBefore) {
            pop_stack(stack);
        }

        A6(t,uk,0,newValue);

        if (INTERPRET_FLAG) {        

            value->isAssignable = 0;
            switch(T) {
   
            case TMul: { value->dataAsInt64 = value->dataAsInt64 * newValue->dataAsInt64; };break;
            case TRest: { value->dataAsInt64 = value->dataAsInt64 % newValue->dataAsInt64; };break;
            case TDiv: { 
                if (newValue->dataAsInt64 == 0) {
                    printf ("Devision by zero (uk: %d)",  *uk);
                    exit(EXIT_FAILURE);
                }
                value->dataAsInt64 = value->dataAsInt64 / newValue->dataAsInt64; 
            };break;

            }
        }
        oldUk = *uk;
        T = scan(lex,t,uk);

    }

    if (INTERPRET_FLAG) {        
        free(newValue);
    }
    *uk = oldUk;

}

void A6(char *t,int *uk,NodeStack *stack,  Node *value) {

    int oldUk = *uk, T = scan(lex,t,uk);
    int idUk = *uk;
    char *id = (char *)calloc(strlen(lex), sizeof(char));
    strcpy(id, lex);


    if (T == TCrBrackOpen){

        Expression(t,uk,value);

        oldUk = *uk;
        T = scan(lex,t,uk);

        if (T != TCrBrackClose){
            printf ("Expected  `)`, got: %x, uk: %d, lex: %s", T, *uk, lex);
            exit(EXIT_FAILURE);
        } 

    } else if (T == Tid) {

        Node *declaredVar = findUp(current, id, FALSE);
        
        if ( !declaredVar ) {
            printf ("ERROR: Variable %s is not declared. [id: %d] \n", id, idUk);
            exit(EXIT_FAILURE);
        }

        short isVar = (declaredVar->type == dTInt || declaredVar->type == dTInt64);
        short isArray =  declaredVar->elementsCount > 0;

        if (!isVar) {
            printf ("ERROR:  %s is declared but it is not a variable, it's a function. [uk: %d] \n", id, idUk);
            exit(EXIT_FAILURE);
        }

        oldUk = *uk;
        T = scan(lex,t,uk);
        
        if (T == TSqBrackOpen) {

            Node *index;

            if (INTERPRET_FLAG) {
                index = malloc(sizeof(Node));
            }

            Expression(t,uk, index);
            T = scan(lex,t,uk);
            
            if (T != TSqBrackClose) {
                printf ("Expected  `]`, got: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            }
            
            if (!isArray) {
                printf ("ERROR:  %s is not array. [uk: %d] \n", id, idUk);
                exit(EXIT_FAILURE);
            }

            if (INTERPRET_FLAG) {
                if (index->dataAsInt > declaredVar->elementsCount - 1 || index->dataAsInt64 < 0) {
                    printf ("%d %d\n", index->dataAsInt64, declaredVar->elementsCount-1);
                    printf ("Index of array `%s` is out of bounds. uk: %d, lex: %s", declaredVar->id, *uk, lex);
                    exit(EXIT_FAILURE);
                }

                switch(declaredVar->type) {

                case dTInt: { value->dataAsInt = declaredVar->dataAsIntArray[index->dataAsInt]; }; break;
                case dTInt64: { value->dataAsInt64 = declaredVar->dataAsInt64Array[index->dataAsInt]; }; break;

                }
            
                value->type = declaredVar->type;
                value->isAssignable = 1;

                if (stack != 0) {
                    push_stack(stack,declaredVar,index->dataAsInt );
                }
                free(index);
            }
            
        } else {
            
            if (isArray) {
                printf ("ERROR:  %s is declared but it is not a variable, it's an array. [uk: %d] \n", id, idUk);
                exit(EXIT_FAILURE);
            }

            *uk = oldUk;

            if (INTERPRET_FLAG) {
                value->type = declaredVar->type;
                value->dataAsInt64 = declaredVar->dataAsInt64;

                value->isAssignable = 1;

                if (stack != 0) {
                    push_stack(stack,declaredVar,-1);
                }
            }

        }

    } else if ( T != TConstDec && T != TConstHex) {
        printf ("Expected  `(Expr)` or `id` or `id[expr]` or `const`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);

    } else {

        if (INTERPRET_FLAG) {
            int base = (T == TConstDec) ? 10 : 16;

            value->dataAsInt64 = strtol(lex,0,base);

            value->type = (value->dataAsInt64 >= INT_MAX) 
                ?  dTInt64 
                :  dTInt; 

            value->isAssignable = 0;
        }
    }
}

void For(char *t,int *uk){

    int oldUk = *uk, T = scan(lex,t,uk);
    Node * value = malloc(sizeof(Node));        

    if (T != TFor) {
        printf ("Expected  `<for>`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    T = scan(lex,t,uk);
    if (T != TCrBrackOpen) {
        printf ("Expected  `(`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    Expression(t,uk,value);

    T = scan(lex,t,uk);
    if (T != TComma) {
        printf ("Expected  `;`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    Expression(t,uk,value);

    T = scan(lex,t,uk);
    if (T != TComma) {
        printf ("Expected  `;`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }
    
    Expression(t,uk,value);

    T = scan(lex,t,uk);
    if (T != TCrBrackClose) {
        printf ("Expected  `)`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    Operator(t,uk);

}
