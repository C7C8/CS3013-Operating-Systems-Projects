CFLAGS=-g -pthread

all: main.c node_normal node_noisemaker

clean:
	rm -f main
	rm -f *.o

node_normal: node node_normal.h node_normal.c params.h

node_noisemaker: node node_noisemaker.h node_noisemaker.c params.h

node: node.h node.c
