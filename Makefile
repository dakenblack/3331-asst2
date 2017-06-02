CC=gcc
CFLAGS=-g -std=gnu99 -Wall
OFLAGS=-lpthread
DEPS=

all: Dvr

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $< 

Dvr: Dvr.o distance_vector.o
	$(CC) $(OFLAGS) -o $@ Dvr.o distance_vector.o

temp : temp.o
	$(CC) $(OFLAGS) -o $@ $< 

clean: 
	rm *.o temp client server
