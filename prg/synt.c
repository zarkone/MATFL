#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <scaner.h>
#include <synt.h>

void Prg(char *t, int *uk){

    lex=(char *)calloc(MAXLEX,sizeof(char));	
    int oldUk = *uk, T = scan(lex, t, uk);

	while(T == TInt || T == TInt64 || T == TVoid || T == TEnd) {

        if (T == TEnd) {
            printf ("\n -*- Exit Success -*-\n uk: %d, lex: %s, T: %x \n", *uk, lex, T);
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

	int	oldUk,
		T = scan(lex, t, uk),
		T1 = scan(lex, t, uk);
	
	if (!((T == TVoid  && T1 == Tid) || (T == TInt && T1 == TMain))) {
       printf("Expected `void <id>` or `int main`, got: %x (uk: %d, lex: %s)", T, *uk, lex);
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

	int	oldUk,
		T = scan(lex, t, uk);

    if (!(T == TInt || T == TInt64)) {
        printf ("Expected `int` or `__int64` , got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }
    
    do {

        T = scan(lex, t, uk);
        if (T != Tid) {
            printf ("Expected `<id>`, got: %x, uk: %d, lex: %s", T, *uk, lex);
            exit(EXIT_FAILURE);
        }
        
        T = scan(lex, t, uk);
        if (T == TSqBrackOpen) {

            
            T = scan(lex, t, uk);
            if (!(T == TConstDec || T == TConstHex)) {
                printf ("Expected `DecConst` or `HexConst`, got: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            }
                    
            T = scan(lex, t, uk);
            if (T != TSqBrackClose) {
                printf ("Expected `]`, got: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            }

            PossibleArrInit(t,uk);
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

}
void Operator(char *t,int *uk){

    int oldUk = *uk, T = scan(lex,t,uk);

    if (T == TFor) {

        *uk = oldUk;
        For(t,uk);
        return;
        
    } else if (T == TEgBrackOpen) {

        *uk = oldUk;

        Block(t,uk);
        return;
        
    }
    
    if (T == Tid) {
        T = scan(lex,t,uk);

        if (T == TCrBrackOpen) {

            T = scan(lex,t,uk);

            if (T != TCrBrackClose) {
                printf ("Expected `<id>()`, got: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            } else {

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

void PossibleArrInit(char *t,int *uk){
    
    int oldUk = *uk, T = scan(lex,t,uk);

    if (T != TAssign) { *uk = oldUk; return; }

    T = scan(lex,t,uk);
    if (T != TEgBrackOpen) {
        printf ("Expected  `{`, got: %x, uk: %d, lex: %s", T, *uk, lex);
        exit(EXIT_FAILURE);
    }

    do {
        
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

    if (T == TCrBrackOpen){

        Expression(t,uk);

        oldUk = *uk;
        T = scan(lex,t,uk);
        
        if (T != TCrBrackClose){

            printf ("Expected  `)`, got: %x, uk: %d, lex: %s", T, *uk, lex);
            exit(EXIT_FAILURE);
            
        } else { }
        
        
    } else if (T == Tid) {

        oldUk = *uk;
        T = scan(lex,t,uk);
        
        if (T == TSqBrackOpen){

            Expression(t,uk);
            
            T = scan(lex,t,uk);
            if (T != TSqBrackClose){
                printf ("Expected  `]`, got: %x, uk: %d, lex: %s", T, *uk, lex);
                exit(EXIT_FAILURE);
            }
            
        } else { *uk = oldUk; }
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
