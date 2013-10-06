#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

int uk=0; 
char* t;

int scan(char* lex){
	int i = 0;

/* skip whitespace & comments:  */
Skip: 
	while(t[uk] == ' ' || t[uk] == '\t' || t[uk] == '\n') uk++;
	if(t[uk] == '/' && t[uk+1] == '/') {
		while(t[uk] != '\n') uk++;
		goto Skip;
	}
	if(t[uk] == '\0') {
		lex[0] = t[uk];
		return TEnd;
	}

/* TId: */
	if(t[uk] >= 'a' && t[uk] <= 'z' || t[uk] >= 'A' && t[uk] <= 'Z'){
		
		while( t[uk] >= 'a' && t[uk] <= 'z' || t[uk] >= 'A' && t[uk] <= 'Z' || t[uk] >= '0' && t[uk] <= '9'  )
		{
			lex[i++] = t[uk++];
		}	
		
		lex[i] = '\0';
		return Tid;
	}


/* TConstHex: */
	if(t[uk] == '0' && t[uk+1] == 'x'){
		uk+=2;
		while( t[uk] >= '0' && t[uk] <= '9' ||  t[uk] >= 'a' && t[uk] <= 'f' || t[uk] >= 'A' && t[uk] <= 'F'  )
			lex[i++] = t[uk++];
		
		if(i == 2) {
			lex[0] = '\0';
			return TError;
		}
		lex[i] = '\0';
		return TConstHex;
	}
	
/* TConstDec: */
	if(t[uk] >= '0' && t[uk] <= '9'){
		
		while( t[uk] >= '0' && t[uk] <= '9'  )
			lex[i++] = t[uk++];
		
		lex[i] = '\0';
		return TConstDec;
	}  

/* Brackets: */
	if(t[uk] == '[') { lex[i++] = t[uk++]; lex[i] = '\0';return TSqBrackOpen; }
	if(t[uk] == ']') { lex[i++] = t[uk++]; lex[i] = '\0';return TSqBrackClose; }
	if(t[uk] == '(') { lex[i++] = t[uk++]; lex[i] = '\0';return TCrBrackOpen; }
	if(t[uk] == ')') { lex[i++] = t[uk++]; lex[i] = '\0';return TCrBrackClose; }
	if(t[uk] == '{') { lex[i++] = t[uk++]; lex[i] = '\0';return TEgBrackOpen; }
	if(t[uk] == '}') { lex[i++] = t[uk++]; lex[i] = '\0';return TEgBrackClose; }
// 
	if(t[uk] == '+') { lex[i++] = t[uk++]; lex[i] = '\0';return TPlus; }
	if(t[uk] == '-') { lex[i++] = t[uk++]; lex[i] = '\0';return TMinus; }
	if(t[uk] == '/') { lex[i++] = t[uk++]; lex[i] = '\0';return TDiv; }
	if(t[uk] == '*') { lex[i++] = t[uk++]; lex[i] = '\0';return TMul; }
	if(t[uk] == '%') { lex[i++] = t[uk++]; lex[i] = '\0';return TRest; }

	if(t[uk] == '>') { 
		lex[i++] = t[uk++];  
		if(t[uk] == '=') { lex[i++] = t[uk]; lex[i] = '\0'; return TGrEq; }
		else {  lex[i] = '\0'; return TGr;}
	}
	if(t[uk] == '<') { 
		lex[i++] = t[uk++];  
		if(t[uk] == '=') { lex[i++] = t[uk]; lex[i] = '\0'; return TLessEq; }
		else {  lex[i] = '\0'; return TLess;}
	}
	if(t[uk] == '!') { 
		lex[i++] = t[uk++];  
		if(t[uk] == '=') { lex[i++] = t[uk]; lex[i] = '\0'; return TNotEq; }
		else {  lex[i] = '\0'; return TNot;}
	}

	uk++;
	return -1;

}


int main(int argc, char** argv){

	if (argc < 2) {
		printf("Usage: ./scanner <file.cpp> \n" );
		return 1;
	} 
	
	FILE *fp = fopen (argv[1],"r");;
	size_t len = 0;
	ssize_t read;
	
	if (fp == NULL){
		printf("Cannot open file: %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	
	char *line = NULL;
	int fileLen = 0;

	t = calloc(1,sizeof(char));
	
	while ((read = getline(&line, &len, fp)) != -1) {

		t = (char *)realloc(t, (fileLen + len + 1) * sizeof(char));
		fileLen += len;

		t = strcat(t, line);
	}
	t[fileLen] = '\0';
	
	char *lex=(char *)calloc(MAXLEX,sizeof(char));
	int type = 0;

	while((type = scan(lex)) != TEnd){
		
		// TODO: remove when all table will be translated
		if(type == -1) continue;
		
		printf("%x: %s (%d)\n", type, lex, uk);
		lex[0] = '\0';

	}

    exit(EXIT_SUCCESS);
}
