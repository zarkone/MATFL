
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <scaner.h>
#include <synt.h>
#include <semant.h>

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
                case dTInt64: { current->dataAsInt64Array = (int64_t *)calloc(current->elementsCount, sizeof(int64_t)); };break;

                }
            }

            PossibleArrInit(t,uk);
            T = scan(lex, t, uk);
            
        } else if (T == TAssign) {
            Node * value;
            value = Expression(t,uk);
            
            if (INTERPRET_FLAG) {
                if (typeCast(current, value) < 0) {

                    printf ("Expected int and __int64 types to cast, got: %d, %d\n", current->type, value->type);
                    printf (" uk: %d, lex: %s",  *uk, lex);
                    printf ("%s %d\n",value->id, value->dataAsInt64);
                    exit(EXIT_FAILURE);
                }
            }

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
            } else {
                
                Node *declaredFunction = findUp(current, id, FALSE);
                
                if ( !declaredFunction ) {
                    printf ("ERROR: Function %s is not declared. [uk: %d] \n", id, idUk);
                    exit(EXIT_FAILURE);
                }

                BOOL isFunction = (declaredFunction->type == dTVoidFunc || declaredFunction->type == dTIntMain);
                if (!isFunction) {
                    printf ("ERROR:  %s is declared but it is not a function. [uk: %d] \n", id, idUk);
                    exit(EXIT_FAILURE);
                }
                    

                T = scan(lex,t,uk);
            }

        } else {
            
            *uk = oldUk;
            Expression(t,uk);
            T = scan(lex,t,uk);
        }
        
    } else if (T == TCrBrackOpen ||
               T == TConstDec ||
               T == TConstHex ) {

        *uk = oldUk;
        Expression(t,uk);
        T = scan(lex,t,uk);
     
    }
    
    if (T != TComma) {
        printf ("Expected  `;`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }
    
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

        int base = (T == TConstDec) ? 10 : 16;

        switch(current->type)
        {
        case dTInt: { current->dataAsIntArray[current->elementsCount - eIndex] = strtol(lex,0,base); }; break;
        case dTInt64: { current->dataAsInt64Array[current->elementsCount - eIndex] = strtol(lex,0,base); }; break;
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

Node * Expression(char *t,int *uk){

    int oldUk = *uk, T;
    Node *value;
    NodeStack *stack;


    init_stack(stack, 128); 

    do {
        
        value = A2(t,uk, stack);
        oldUk = *uk;
        T = scan(lex,t,uk);


    } while(T == TAssign);
    
    if (stack->length > 0) {
        Node *var;
        
        while(var = pop_stack(stack)) {

            if (var->elementsCount > 0) {

                switch(var->type) {

                case dTInt: { var->dataAsIntArray[var->currentArrayIndex] = value->dataAsInt;  }; break;
                case dTInt64: { var->dataAsInt64Array[var->currentArrayIndex] = value->dataAsInt64;  }; break;

                }

            }
            else {
                var->dataAsInt64 =  value->dataAsInt64;
            }

        }
    }

    *uk = oldUk;
    
    return value;
    
}

Node * A2(char *t,int *uk, NodeStack *stack){

    int oldUk = *uk;
    Node * value;

    value = A3(t,uk,stack);

    oldUk = *uk;
    int T = scan(lex,t,uk);

    while (T == TAnd || T == TOr) {

        Node * newValue = A3(t,uk,0);

        if (T == TAnd) {
            value->dataAsInt = value->dataAsInt && newValue->dataAsInt;
        } else {
            value->dataAsInt = value->dataAsInt || newValue->dataAsInt;
        }

        oldUk = *uk;
        T = scan(lex,t,uk);
    }

    *uk = oldUk;

    return value;
}
Node * A3(char *t,int *uk, NodeStack *stack){

    int oldUk = *uk, isNot = 0;

    int T = scan(lex,t,uk);
    
    if (T != TNot) { *uk = oldUk; }
    else { isNot = 1; }
    
    Node *value;
    value = A4(t,uk,stack);

    if (isNot) { value->dataAsInt = !value->dataAsInt; }

    oldUk = *uk;
    T = scan(lex,t,uk);
    
    while (T == TGrEq || T == TGr || T == TLess || T == TLessEq || T == TEq || T == TNotEq) {

        Node *newValue = A4(t,uk,0);

        switch(T) {
   
        case TGrEq: { value->dataAsInt = value->dataAsInt >= newValue->dataAsInt; };break;
        case TGr: { value->dataAsInt = value->dataAsInt > newValue->dataAsInt; };break;
        case TLess: { value->dataAsInt = value->dataAsInt < newValue->dataAsInt; };break;
        case TLessEq: { value->dataAsInt = value->dataAsInt <= newValue->dataAsInt; };break;
        case TEq: { value->dataAsInt = value->dataAsInt == newValue->dataAsInt; };break;
        case TNotEq: { value->dataAsInt = value->dataAsInt != newValue->dataAsInt; };break;

        }

        oldUk = *uk;
        T = scan(lex,t,uk);

    }
    
    *uk = oldUk;

    return value;
}

Node * A4(char *t,int *uk, NodeStack *stack){

    int oldUk = *uk, isNegative = 0;

    int T = scan(lex,t,uk);
    if (T != TPlus && T != TMinus) { *uk = oldUk; }
    else if (T == TMinus) { isNegative = 1; }

    Node *value;
    value = A5(t,uk,stack);
    if (isNegative) { value->dataAsInt64 = -value->dataAsInt64; }

    oldUk = *uk;
    T = scan(lex,t,uk);
    
    while (T == TPlus || T == TMinus ) {

        Node *newValue = A5(t,uk,0);
        switch(T) {
   
        case TPlus: { value->dataAsInt = value->dataAsInt + newValue->dataAsInt; };break;
        case TMinus: { value->dataAsInt = value->dataAsInt - newValue->dataAsInt; };break;

        }
        
        oldUk = *uk;
        T = scan(lex,t,uk);

    }
    
    *uk = oldUk;

    return value;
}

Node * A5(char *t,int *uk, NodeStack *stack){

    int oldUk = *uk;
    Node *value;
    value = A6(t,uk, stack);

    oldUk = *uk;
    int T = scan(lex,t,uk);
    
    while (T == TMul || T == TDiv || T == TRest ) {
        Node *newValue = A6(t,uk,0);
        switch(T) {
   
        case TMul: { value->dataAsInt = value->dataAsInt * newValue->dataAsInt; };break;
        case TRest: { value->dataAsInt = value->dataAsInt % newValue->dataAsInt; };break;
        case TDiv: { 
            if (newValue->dataAsInt == 0) {
                printf ("Devision by zero (uk: %d)",  *uk);
                exit(EXIT_FAILURE);
            }
            value->dataAsInt = value->dataAsInt / newValue->dataAsInt; 
        };break;

        }

        oldUk = *uk;
        T = scan(lex,t,uk);

    }
    
    *uk = oldUk;

    return value;

}

Node * A6(char *t,int *uk, NodeStack *stack) {

    int oldUk = *uk, T = scan(lex,t,uk);
    int idUk = *uk;
    char *id = (char *)calloc(strlen(lex), sizeof(char));
    strcpy(id, lex);
    Node * value = (Node *)calloc(1, sizeof(Node));        

    if (T == TCrBrackOpen){
        
        value = Expression(t,uk);
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

        BOOL isVar = (declaredVar->type == dTInt || declaredVar->type == dTInt64);
        BOOL isArray =  declaredVar->elementsCount > 0;

        if (!isVar) {
            printf ("ERROR:  %s is declared but it is not a variable, it's a function. [uk: %d] \n", id, idUk);
            exit(EXIT_FAILURE);
        }

        oldUk = *uk;
        T = scan(lex,t,uk);
        
        if (T == TSqBrackOpen) {

            Node * index = Expression(t,uk);
            T = scan(lex,t,uk);
            
            if (T != TSqBrackClose) {
                printf ("Expected  `]`, got: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            }
            
            if (!isArray) {
                printf ("ERROR:  %s is not array. [uk: %d] \n", id, idUk);
                exit(EXIT_FAILURE);
            }
            
            switch(declaredVar->type) {

            case dTInt: { value->dataAsInt = declaredVar->dataAsIntArray[index->dataAsInt]; }; break;
            case dTInt64: { value->dataAsInt64 = declaredVar->dataAsInt64Array[index->dataAsInt]; }; break;

            }
            
            value->type = declaredVar->type;
            declaredVar->currentArrayIndex = index->dataAsInt;

            if (stack != 0) {
                push_stack(stack,declaredVar);
            }

            
        } else {
            
            if (isArray) {
                printf ("ERROR:  %s is declared but it is not a variable, it's an array. [uk: %d] \n", id, idUk);
                exit(EXIT_FAILURE);
            }

            *uk = oldUk;
            value->type = declaredVar->type;
            value->dataAsInt64 = declaredVar->dataAsInt64;

            if (stack != 0) {
                push_stack(stack,declaredVar);
            }

        }

    } else if ( T != TConstDec && T != TConstHex) {
        printf ("Expected  `(Expr)` or `id` or `id[expr]` or `const`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);

    } else {

        int base = (T == TConstDec) ? 10 : 16;

        value->dataAsInt = strtol(lex,0,base);
        value->type = dTInt;


    }

    return value;

}

void For(char *t,int *uk){

    int oldUk = *uk, T = scan(lex,t,uk);

    if (T != TFor) {
        printf ("Expected  `<for>`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    T = scan(lex,t,uk);
    if (T != TCrBrackOpen) {
        printf ("Expected  `(`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    Expression(t,uk);

    T = scan(lex,t,uk);
    if (T != TComma) {
        printf ("Expected  `;`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    Expression(t,uk);

    T = scan(lex,t,uk);
    if (T != TComma) {
        printf ("Expected  `;`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }
    
    Expression(t,uk);

    T = scan(lex,t,uk);
    if (T != TCrBrackClose) {
        printf ("Expected  `)`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    Operator(t,uk);

}
