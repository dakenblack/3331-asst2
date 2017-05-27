CC=gcc
CFLAGS=-g -std=gnu99 -Wall
OFLAGS=-lpthread
DEPS=

all: client server Dvr

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $< 

Dvr: Dvr.o
	$(CC) $(OFLAGS) -o $@ $< 

client: client.o
	$(CC) $(OFLAGS) -o $@ $< 

server: server.o
	$(CC) $(OFLAGS) -o $@ $< 

temp : temp.o
	$(CC) $(OFLAGS) -o $@ $< 

clean: 
	rm *.o temp client server
