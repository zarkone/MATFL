
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <scaner.h>
#include <synt.h>
#include <semant.h>

Node *current;

void Prg(char *t, int *uk){

    lex=(char *)calloc(MAXLEX,sizeof(char));	
    int oldUk = *uk, T = scan(lex, t, uk);

    Node *root = (Node *)calloc(1, sizeof(Node));
    root->type = dBlock;
    root->isAssignable = FALSE;
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


    Block(t, uk);


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
            
            
            PossibleArrInit(t,uk, current->elementsCount);
            T = scan(lex, t, uk);
            
        } else if (T == TAssign) {

            Expression(t,uk);
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

void PossibleArrInit(char *t,int *uk, int elementsCount){

    
    int oldUk = *uk, T = scan(lex,t,uk);

    if (T != TAssign) { *uk = oldUk; return; }

    T = scan(lex,t,uk);
    if (T != TEgBrackOpen) {
        printf ("Expected  `{`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    do {
        
        elementsCount--;

        T = scan(lex,t,uk);

        if (T != TConstDec && T != TConstHex) {
            printf ("Expected  `ConstDec` or `ConstHex`, got: %x, uk: %d, lex: %s", T, *uk, lex);
            exit(EXIT_FAILURE);
        }
        
        T = scan(lex,t,uk);
        if (T == TEgBrackClose) { break; }

    } while(T == TSem);
    
    if (T != TEgBrackClose) {
        printf ("Expected  `}`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    if (elementsCount < 0) {
        printf ("To few elements in array init. uk: %d ", *uk);
        exit(EXIT_FAILURE);
    }
}

void Expression(char *t,int *uk){

    int oldUk = *uk, T;

    do {
        
        A2(t,uk);
        oldUk = *uk;
        T = scan(lex,t,uk);

    } while(T == TAssign);

    *uk = oldUk;
    
}

void A2(char *t,int *uk){

    int oldUk = *uk;

    A3(t,uk);
    oldUk = *uk;
    int T = scan(lex,t,uk);
    
    while (T == TAnd || T == TOr) {
        A3(t,uk);
        oldUk = *uk;
        T = scan(lex,t,uk);
    }
    *uk = oldUk;
    
}
void A3(char *t,int *uk){

    int oldUk = *uk;

    int    T = scan(lex,t,uk);
    if (T != TNot) { *uk = oldUk; }
    
    A4(t,uk);
    oldUk = *uk;
    T = scan(lex,t,uk);
    
    while (T == TGrEq || T == TGr || T == TLess || T == TLess || T == TEq || T == TNotEq) {
        A4(t,uk);
        oldUk = *uk;
        T = scan(lex,t,uk);

    }
    
    *uk = oldUk;
}

void A4(char *t,int *uk){

    int oldUk = *uk;

    int T = scan(lex,t,uk);
    if (T != TPlus && T != TMinus) { *uk = oldUk; }
    
    A5(t,uk);
    oldUk = *uk;
    T = scan(lex,t,uk);
    
    while (T == TPlus || T == TMinus ) {
        A5(t,uk);
        oldUk = *uk;
        T = scan(lex,t,uk);

    }
    
    *uk = oldUk;
}

void A5(char *t,int *uk){

    int oldUk = *uk;

    A6(t,uk);
    oldUk = *uk;
    int T = scan(lex,t,uk);
    
    while (T == TMul || T == TDiv || T == TRest ) {
        A6(t,uk);
        oldUk = *uk;
        T = scan(lex,t,uk);

    }
    
    *uk = oldUk;
}
void A6(char *t,int *uk){


    int oldUk = *uk, T = scan(lex,t,uk);
    int idUk = *uk;
    char *id = (char *)calloc(strlen(lex), sizeof(char));
    strcpy(id, lex);
        
    if (T == TCrBrackOpen){

        Expression(t,uk);
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
        
        if (T == TSqBrackOpen){

            Expression(t,uk);
            T = scan(lex,t,uk);
            
            if (T != TSqBrackClose) {
                printf ("Expected  `]`, got: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            }
            
        } else {
            
            if (isArray) {
                printf ("ERROR:  %s is declared but it is not a variable, it's an array. [uk: %d] \n", id, idUk);
                exit(EXIT_FAILURE);
            }

            *uk = oldUk;
        }
    } else if ( T != TConstDec && T != TConstHex) {
        printf ("Expected  `(Expr)` or `id` or `id[expr]` or `const`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);

    } 
    


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
