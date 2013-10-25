#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <scaner.h>
#include <synt.h>

void Prg(char *t, int *uk){
	
    int oldUk = *uk, T = scan(lex, t, uk);
	*uk = oldUk;

	while(T == TInt || T == TInt64 || T == TVoid){

		if (T == TVoid) { FuncDescr(t, uk); }
		else if (T == TInt64) { VarDescr(t, uk); }
		else if (T == TInt) {

			T = scan(lex, t, uk); *uk = oldUk;
			if (T == TMain) { FuncDescr(t, uk);  }
			else if (T == Tid) { VarDescr(t, uk); }
			else {
                printf ("Expected `<id>` or `<main>`, got: %x (uk: %d)", T, *uk);
                exit(EXIT_FAILURE);
            }

		} else {
            printf ("Expected `<function descr>` or `<var descr>`, got: %x (uk: %d)", T, *uk);
            exit(EXIT_FAILURE);
        }
	}
	
}

void FuncDescr(char *t, int *uk){

	int	oldUk,
		T = scan(lex, t, uk),
		T1 = scan(lex, t, uk);
	
	if (!((T == TVoid  && T1 == Tid) || (T == TInt && T1 == TMain))) {
       printf("Expected `void <id>` or `int main`, got: %x (uk: %d)", T, *uk);
       exit(EXIT_FAILURE);
    }
        
    T = scan(lex, t, uk);
    T1 = scan(lex, t, uk);

    if (!(T == TCrBrackOpen && T1 == TCrBrackClose)) {
        printf ("Expected `()`, got: %x (uk: %d)", T, *uk);
        exit(EXIT_FAILURE);
    }

    oldUk = *uk;
    T = scan(lex, t, uk);
    *uk = oldUk;

    if (!(T == TEgBrackOpen)) {
        printf ("Expected `{ `, got: %x (uk: %d)", T, *uk);
        exit(EXIT_FAILURE);
    }
    Block(t, uk); 


    
	
}

void VarDescr(char *t, int *uk){

	int	oldUk,
		T = scan(lex, t, uk);
    
    if (!(T == TInt || T == TInt64)) {
        printf ("Expected `int` or `__int64` , got: %x, uk: %d", T, *uk);
        exit(EXIT_FAILURE);
    }

    T = scan(lex, t, uk);
    if (T != Tid) {
        printf ("Expected `<id>`, got: %x, uk: %d", T, *uk);
        exit(EXIT_FAILURE);
    }

    T = scan(lex, t, uk);
    if (T == TSqBrackOpen){

        T = scan(lex, t, uk);
        if (!(T == TConstDec || T == TConstHex)) {
            printf ("Expected `DecConst` or `HexConst`, got: %x, uk: %d", T, *uk);
            exit(EXIT_FAILURE);
        }
                    
        T = scan(lex, t, uk);
        if (T != TSqBrackClose) {
            printf ("Expected `]`, got: %x, uk: %d", T, *uk);
            exit(EXIT_FAILURE);
        }

        PossibleArrInit(t,uk);
        
    } /*  = ; , */
                

        
}

void Block(char *t, int *uk){
	printf ("%x\n",scan(lex, t, uk));
	printf ("Block. \n");
}
void PossibleArrInit(char *t,int *uk){

}
