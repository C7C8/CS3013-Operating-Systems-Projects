CFLAGS=-g -pthread

all: main.c node noisemaker

clean:
	rm -f main
	rm -f *.o

node: node.h node.c params.h

noisemaker: node.h noisemaker.c params.c


