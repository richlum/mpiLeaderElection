CC=mpicc
CFLAGS=-g -Wall
DFLAG=-DDEBUG
electleader: electleader.c electleader.h
	$(CC) $(CFLAGS) $^ -o electleader

all:	electleader

clean:
	rm electleader 
run:
	mpiexec -n 4 ./electleader


debug:  electleader.c
	$(CC) $(DFLAG) $(CFLAGS) $^ -o electleader

