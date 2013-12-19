run: main
	./compile test.cpp
	perl make-tree.pl < semant-tree.txt > semant-tree.tree
main: buildlib
	cc -I./ -g -Wall compile.c -L./ -lcompile -o compile

buildlib: scaner synt semant
	ar ruv libcompile.a scaner.o synt.o semant.o
	ranlib libcompile.a

semant: synt
	cc  -I./ -g -c semant.c 
synt: scaner
	cc  -I./ -g -c synt.c 
scaner:
	cc  -I./ -g -c scaner.c 

clean:
	rm -f  *.o *.a compile
tags:
	rm -f TAGS
	find . | grep ".*\.\(h\|c\)" | xargs etags -f TAGS
