CC = gcc
inputfile = test.cpp

run-tree: tags run tree 
run: main 
	./compile $(inputfile)

main: buildlib
	$(CC) -I./ -g -Wall compile.c -L./ -lcompile -o compile

buildlib: semant
	ar ruv libcompile.a scaner.o synt.o semant.o
	ranlib libcompile.a

semant: synt
	$(CC) -I./ -g -c semant.c 
synt: scaner
	$(CC) -I./ -g -c synt.c 
scaner:
	$(CC) -I./ -g -c scaner.c 
clean:
	rm -f  *.o *.a compile
tags:
	rm -f TAGS
	find . | grep ".*\.\(h\|c\)$$" | xargs etags -f TAGS

tree:
	perl make-tree.pl < semant-tree.txt > semant-tree.tree 


test1: 	inputfile = test.cpp
test1:	run;


test2: 	inputfile = test2.cpp
test2:	run-tree;

