CFLAGS=-g -pthread -Wall -Wextra

all: main.c node_normal.o node_noisemaker.o

clean:
	rm -f main
	rm -f *.o

node_normal.o: node.o node_normal.h node_normal.c params.h

node_noisemaker.o: node.o node_noisemaker.h node_noisemaker.c params.h

node.o: node.h node.c
