#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <scaner.h>

int scan(char* lex, char* t, int *uk){
	int i = 0;


/* skip whitespace & comments:  */
Skip: 
	while(t[*uk] == ' ' || t[*uk] == '\t' || t[*uk] == '\n') (*uk)++;
	if(t[*uk] == '/' && t[*uk+1] == '/') {
		while(t[*uk] != '\n') (*uk)++;
		goto Skip;
	}
	if(t[*uk] == '\0') {
		lex[0] = t[*uk];
		return TEnd;
	}

/* TId: */
	if(t[*uk] == '_' || t[*uk] >= 'a' && t[*uk] <= 'z' || t[*uk] >= 'A' && t[*uk] <= 'Z'){


		while(t[*uk] == '_' || t[*uk] >= 'a' && t[*uk] <= 'z' || t[*uk] >= 'A' && t[*uk] <= 'Z' || t[*uk] >= '0' && t[*uk] <= '9'  ){

			if(i >= MAXLEX) { (*uk)++; continue; };

			lex[i++] = t[(*uk)++];
        }
        
        if(i >= MAXLEX) printf ("Warn: Too long ID (%d)\n", *uk);

		lex[i] = '\0';

        if(!strcmp(lex, "void")) { return TVoid; }
        if(!strcmp(lex, "int")) { return TInt; }
        if(!strcmp(lex, "__int64")) { return TInt64; }
        if(!strcmp(lex, "main")) { return TMain; }
        if(!strcmp(lex, "for")) { return TFor; }

		return Tid;
	}


/* TConstHex: */
	if(t[*uk] == '0' && (t[*uk+1] == 'x' || t[*uk+1] == 'X')){
		*uk+=2;
		while( t[*uk] >= '0' && t[*uk] <= '9' ||  t[*uk] >= 'a' && t[*uk] <= 'f' || t[*uk] >= 'A' && t[*uk] <= 'F'  ){

            if(i >= MAXLEX) { (*uk)++; continue; };

			lex[i++] = t[(*uk)++];
        }

        if(i >= MAXLEX) printf ("Warn: Too long const (%d)\n", *uk);

		if(i == 0) {

            printf ("Empty 0x const\n");
			lex[0] = '\0';
			return TError;
		}

		lex[i] = '\0';
		return TConstHex;
	}
	
/* TConstDec: */
	if(t[*uk] >= '0' && t[*uk] <= '9'){
		
		while( t[*uk] >= '0' && t[*uk] <= '9'  )
			lex[i++] = t[(*uk)++];
		
		lex[i] = '\0';
		return TConstDec;
	}  

/* Brackets: */
	if(t[*uk] == '[') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TSqBrackOpen; }
	if(t[*uk] == ']') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TSqBrackClose; }
	if(t[*uk] == '(') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TCrBrackOpen; }
	if(t[*uk] == ')') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TCrBrackClose; }
	if(t[*uk] == '{') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TEgBrackOpen; }
	if(t[*uk] == '}') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TEgBrackClose; }
 
	if(t[*uk] == '+') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TPlus; }
	if(t[*uk] == '-') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TMinus; }
	if(t[*uk] == '/') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TDiv; }
	if(t[*uk] == '*') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TMul; }
	if(t[*uk] == '%') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TRest; }

	if(t[*uk] == ',') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TSem; }
    if(t[*uk] == ';') { lex[i++] = t[(*uk)++]; lex[i] = '\0';return TComma; }

	if(t[*uk] == '>') { 
		lex[i++] = t[(*uk)++];  
		if(t[*uk] == '=') { lex[i++] = t[*uk]; lex[i] = '\0'; return TGrEq; }
		else {  lex[i] = '\0'; return TGr;}
	}
	if(t[*uk] == '<') { 
		lex[i++] = t[(*uk)++];  
		if(t[*uk] == '=') { lex[i++] = t[*uk]; lex[i] = '\0'; return TLessEq; }
		else {  lex[i] = '\0'; return TLess;}
	}
	if(t[*uk] == '!') { 
		lex[i++] = t[(*uk)++];  
		if(t[*uk] == '=') { lex[i++] = t[*uk]; lex[i] = '\0'; return TNotEq; }
		else {  lex[i] = '\0'; return TNot;}
	}
	if(t[*uk] == '=') { 
		lex[i++] = t[(*uk)++];  
		if(t[*uk] == '=') { lex[i++] = t[*uk]; lex[i] = '\0'; return TEq; }
		else {  lex[i] = '\0'; return TAssign;}
	}

    lex[i++] = t[*uk];

    printf ("Unknown lex (uk: %d, lex: %s)\n",*uk,lex);
    exit(EXIT_FAILURE);

    lex[i] = '\0';
	(*uk)++;


}

