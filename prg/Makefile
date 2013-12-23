run: main 
	./compile test.cpp

main: buildlib
	cc -I./ -g -Wall compile.c -L./ -lcompile -o compile

buildlib: scaner ll1
	ar ruv libcompile.a scaner.o ll1.o
	ranlib libcompile.a

ll1: scaner
	cc  -I./ -g -c ll1.c 
scaner:
	cc  -I./ -g -c scaner.c 
clean:
	rm -f  *.o *.a compile
tags:
	rm -f TAGS
	find . | grep ".*\.\(h\|c\)" | xargs etags -f TAGS
