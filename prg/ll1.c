#include <stdio.h>
#include <stdlib.h>

#include <scaner.h>
#include <defs.h>
#include <ll1.h>

void printStack(){
    printf("Stack: ");
    int i;
    
    for(i =0; i < si; i++) {
        printf("%x ", stack[i]);
    }
    printf(" \n");
}
void reduct(int Vn, int Vt, char *t) {

    if (Vn == TConstDec || Vn == TConstHex) {
        Vn == TConst;
    }
    printf("%x %d %x\n", Vn, si, Vt);

    si--;
    switch (Vn) {

    case TPrg : {
        
        if (Vt == TVoid) {
            
            stack[si++] = TPrg;
            stack[si++] = TBlock;
            stack[si++] = TCrBrackClose;
            stack[si++] = TCrBrackOpen;
            stack[si++] = Tid;
            stack[si++] = TVoid;
            
        } else if (Vt == TInt ) {

            stack[si++] = TPrg;
            stack[si++] = TVarListOrMain;
            stack[si++] = TInt;
            printf ("here.\n");
            
        } else if (Vt == TInt64) {

            stack[si++] = TPrg;
            stack[si++] = TVarList;
            stack[si++] = TInt64;
        
        } else  {

        }
        
    };break;
    case TVarListOrMain : {

        if (Vt == Tid) {
            printf ("here2.\n");
            stack[si++] = TComma;
            stack[si++] = TVarList;
            
        } else if (Vt == TMain) {

            stack[si++] = TBlock;
            stack[si++] = TCrBrackClose;
            stack[si++] = TCrBrackOpen;
            stack[si++] = TMain;
            
        } else if (Vt == TSem) {

            stack[si++] = TVarList;
            stack[si++] = TSem;
            
        } else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
        
    }; break;
    case TVarList : {

        if (Vt == Tid) {
            printf ("TvarList\n");
            stack[si++] = TListTail;
            stack[si++] = TListElem;
            
        } else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    }; break;
    case TListTail : {

        if (Vt == TSem) {
            printf ("TListTail\n");
            stack[si++] = TVarList;
            stack[si++] = TSem;
        } else {

        }
        
    }; break;
    case TListElem : {
        printf("TLIsteleme\n");

        if (Vt == Tid) {
            
            stack[si++] = TPosAssignOrArray;
            stack[si++] = Tid;
            
        } else {
            
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TPosAssignOrArray : {

        if (Vt == TAssign) {
            
            stack[si++] = TPosAss;
            
        } else if (Vt == TSqBrackOpen) {
            
            stack[si++] = TPossArrInit;
            stack[si++] = TArrLen;
            
        } else {

        }
        
    };break;
    case TPossArrInit : {

        if (Vt == TAssign) {
            
            stack[si++] = TEgBrackClose;
            stack[si++] = TConstList;
            stack[si++] = TEgBrackOpen;
            stack[si++] = TAssign;
            
        } else {
            

        }
        
    };break;
    case TArrLen : {

        if (Vt == TSqBrackOpen) {
            

            stack[si++] = TSqBrackClose;
            stack[si++] = TConst;
            stack[si++] = TSqBrackOpen;
            
        } else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TConstList : {

        if (Vt == TConst) {

            stack[si++] = TConstTail;
            stack[si++] = TConst;
            
        } else {
            printf("%x %x", Vt, TConst);
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TConstTail : {

        if (Vt == TSem) {
            
            stack[si++] = TConstList;
            stack[si++] = TSem;
            
        } else {

        }
        
    };break;
    case TPosAss: {

        if (Vt == TAssign) {
            
            stack[si++] = TExpr;            
            stack[si++] = TAssign;
            
        } else {

        }
        
    };break;
    case TBlock: {

        if (Vt == TEgBrackOpen) {
            
            stack[si++] = TEgBrackClose;
            stack[si++] = TOps;
            stack[si++] = TEgBrackOpen;
            
        } else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TOps: {

        if (Vt == Tid ||
            Vt == TConst ||
            Vt == TCrBrackOpen ||
            Vt == TInt ||
            Vt == TInt64 ||
            Vt == TComma ||
            Vt == TEgBrackOpen ||
            Vt == TFor ) {
            
            stack[si++] = TOps;
            stack[si++] = TOp;
            
        } else {

        }
        
    };break;
    case TOp: {

        if (Vt == Tid ) {
            
            int oldUk = uk;
            
            int T1 = scan(&lex, t, &uk);
            int T2 = scan(&lex, t, &uk);

            uk = oldUk;
            
            if (T1 == TCrBrackOpen && T2 == TCrBrackClose) {
                stack[si++] = TComma;
                stack[si++] = TFunCall;
                
            } else if (T1 == TCrBrackOpen && T2 != TCrBrackClose) {

                printf("! LL error: %x\n", Vt);
                exit(EXIT_FAILURE);

            } else {
                stack[si++] = TComma;
                stack[si++] = TExpr;
            }

        } else if (Vt == TConst) {
            stack[si++] = TComma;
            stack[si++] = TExpr;
        }
        else if (Vt == TCrBrackOpen) {
            
            stack[si++] = TComma;
            stack[si++] = TExpr;
        }
        else if (Vt == TInt ) {
            stack[si++] = TComma;
            stack[si++] = TVarList;
            stack[si++] = TInt;
        }
        else if (Vt == TInt64) {
            
            stack[si++] = TComma;
            stack[si++] = TVarList;
            stack[si++] = TInt64;
        }
        else if (Vt == TComma ) {
            stack[si++] = TComma;
        }
        else if (Vt == TEgBrackOpen ) {
            stack[si++] = TBlock;
        }
        else if (Vt == TFor  ) {
            
            stack[si++] = TForCycle;
            
        } else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TExpr: {

        if (Vt == Tid || Vt == TConst || Vt == TCrBrackOpen) {
            stack[si++] = TA1;
        } else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TA1: {

        if (Vt == Tid || Vt == TConst || Vt == TCrBrackOpen) {

            stack[si++] = TA1H;
            stack[si++] = TA2;
            
        } else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TA1H: {

        if (Vt == TAssign) {
            
            stack[si++] = TA1;
            stack[si++] = TAssign;
            
        } else {

        }
        
    };break;
    case TA2: {

        if (Vt == Tid || Vt == TConst || Vt == TCrBrackOpen) {

            stack[si++] = TA2H;
            stack[si++] = TA5;
        } else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TA2H: {

        if (Vt == TAnd){

            stack[si++] = TA5;
            stack[si++] = TAnd;
            
        } else if (Vt == TOr ){

            stack[si++] = TA5;
            stack[si++] = TOr;
        } else {

        }
        
    };break;
    case TA5: {

        if (Vt == Tid || Vt == TConst || Vt == TCrBrackOpen) {

            stack[si++] = TA5H;
            stack[si++] = TA6;
        } else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TA5H: {

        if (Vt == TMul){

            stack[si++] = TA5;
            stack[si++] = TMul;
            
        } else if (Vt == TDiv ){

            stack[si++] = TA5;
            stack[si++] = TDiv;
        }  else if (Vt == TRest ){

            stack[si++] = TA5;
            stack[si++] = TRest;
        } else {

        }
        
    };break;
    case TA6: {

        if (Vt == Tid) {
            

            stack[si++] = TPosInd;
            stack[si++] = Tid;
            
        } else if (Vt == TConst) {

            stack[si++] = TConst;
            
        } else if (Vt == TCrBrackOpen) {

            stack[si++] = TCrBrackClose;
            stack[si++] = TA1;
            stack[si++] = TCrBrackOpen;
            
        } else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TPosInd: {

        if (Vt == TSqBrackOpen){

            stack[si++] = TSqBrackClose;
            stack[si++] = TExpr;
            stack[si++] = TSqBrackOpen;
        }
        else {

        }
        
    };break;
    case TFunCall: {

        if (Vt == Tid){
            
            stack[si++] = TCrBrackClose;
            stack[si++] = TCrBrackOpen;
            stack[si++] = Tid;
        }
        else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;
    case TForCycle: {

        if (Vt == TFor){
            
            stack[si++] = TOp;
            stack[si++] = TCrBrackClose;
            stack[si++] = TExpr;
            stack[si++] = TComma;
            stack[si++] = TExpr;
            stack[si++] = TComma;
            stack[si++] = TExpr;
            stack[si++] = TCrBrackOpen;
            stack[si++] = TFor;
            
        }
        else {
            printf("LL error: %x\n", Vt);
            exit(EXIT_FAILURE);
        }
        
    };break;

    }
}

void ll1Parse (char *t) {
    
    
    si = 0;
    stack[si++] = TEnd;
    stack[si++] = TPrg;

    uk = 0;
    int T = scan(&lex, t, &uk);

    printf("%x: %x\n",TLastVn, TFirstVn);
    
    while (T != TEnd) {

        printStack();
        printf("uk: %d\n", uk);
        if (stack[si-1] >= TFirstVn && stack[si-1] <= TLastVn) {

            reduct(stack[si-1], T, t);

        } else if (stack[si-1] == T) {
            printf("Stack[si-1]: %x\n", stack[si-1]);
            si--;
            T = scan(&lex, t, &uk);
            if (T == TConstDec || T == TConstHex) {
                T = TConst;
            }
            printf ("scan.. %s\n", &lex);
        } else {
            printf ("error\n");
            exit(EXIT_FAILURE);
        }

    }

    printf("\n\t -*- Exit success -*-\n");
    
}
