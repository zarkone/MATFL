main: scaner synt
	cc -I./ -g compile.c scaner.o synt.o -o compile && ./compile test.cpp
synt: scaner
	cc  -I./ -g -c synt.c scaner.o && ar cr synt.a synt.o
scaner:
	cc  -I./ -g -c scaner.c && ar cr scaner.a scaner.o
clean:
	rm  *.o *.a compile
