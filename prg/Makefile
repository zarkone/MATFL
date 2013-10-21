main: scaner
	cc -I./ -g main.c scaner.o -o main && ./main test.cpp
synt: scaner
	cc -g synt.c -o synt
scaner:
	cc  -I./ -g -c scaner.c && ar cr scaner.a scaner.o
clean:
	rm  *.o *.a main
