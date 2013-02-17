CC=mpicc
CFLAGS=-g -Wall
DFLAG=-DDEBUG
electleader: electleader.c electleader.h
	$(CC) $(CFLAGS) $^ -o electleader

all:	electleader

clean:
	rm electleader 
run:
	./setpathandrun

debug:  electleader.c
	$(CC) $(DFLAG) $(CFLAGS) $^ -o electleader

