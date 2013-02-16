CC=mpicc
CFLAGS=-g -Wall

all:	electleader.c
	$(CC) $(CFLAGS) $^ -o electleader

clean:
	rm electleader 
run:
	./setpathandrun
