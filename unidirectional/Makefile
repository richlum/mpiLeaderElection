CC=mpicc
CFLAGS=-g -Wall
DFLAG=-DDEBUG
LFLAG=-lm
electleader: electleader.c electleader.h
	$(CC) $(CFLAGS) $(LFLAG) $^ -o electleader

all:	electleader

clean:
	rm electleader 
run:
	mpiexec -n 11 ./electleader


debug:  electleader.c
	$(CC) $(DFLAG) $(CFLAGS) $(LFLAG) $^ -o electleader

