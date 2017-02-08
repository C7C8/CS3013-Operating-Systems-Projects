CFLAGS=-g -pthread

all: main.c node noisemaker

clean:
	rm -f main
	rm -f *.o

node: node.h node_normal.h node_normal.c params.h

noisemaker: node.h noisemaker.h noisemaker.c params.h


