#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <scaner.h>
#include <synt.h>

int main(int argc, char** argv) {

	if (argc < 2) {
		printf("Usage: ./compile <file.cpp> \n" );
		return 1;
	} 
	char *t = NULL;
	int uk = 0;
	
	FILE *fp = fopen (argv[1],"r");;
	size_t len = 0;
	ssize_t read;

	
	if (fp == NULL){
		printf("Cannot open file: %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    t = malloc(fsize + 1);
    fread(t, fsize, 1, fp);
    fclose(fp);

    t[fsize] = 0;

 
	lex=(char *)calloc(MAXLEX,sizeof(char));
	int type = 0;
	Prg(t, &uk);
    
	// while((type = scan(lex, t, &uk)) != TEnd){

	// 	// TODO: remove when all table will be translated
	// 	// if(type == -1) continue;
		
	// 	printf("%x: \t %s \n", type, lex);
	// 	lex[0] = '\0';

	// }



}
