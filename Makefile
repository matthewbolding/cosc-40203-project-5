# Makefile to compile the pc executable

CC=gcc
CFLAGS=-Wall -pthread -lrt -lm -g

pc: input.o output.o threads.o prod-con.o
	$(CC) -o pc input.o output.o threads.o prod-con.o $(CFLAGS)

clean:
	-rm -f *.o pc

input.o: input.c
	$(CC) $(CFLAGS) -c input.c

output.o: output.c
	$(CC) $(CFLAGS) -c output.c

threads.o: threads.c
	$(CC) $(CFLAGS) -c threads.c

prod-con.o: prod-con.c
	$(CC) $(CFLAGS) -c prod-con.c