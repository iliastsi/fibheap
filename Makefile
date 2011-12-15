.PHONY: clean

all: main.o fibheap.o
	gcc -o test main.o fibheap.o -Wall -Wextra -g

main.o: main.c
	gcc -c main.c -Wall -Wextra -g

fibheap.o: fibheap.c fibheap.h
	gcc -c fibheap.c -Wall -Wextra -g

clean:
	$(RM) *.o test
