run: main
	./compile test.cpp
main: buildlib
	cc -I./ -g -Wall compile.c -L./ -lcompile -o compile

buildlib: scaner synt
	ar ruv libcompile.a scaner.o synt.o
	ranlib libcompile.a

synt: scaner
	cc  -I./ -g -c synt.c 
scaner:
	cc  -I./ -g -c scaner.c 
clean:
	rm -f  *.o *.a compile
