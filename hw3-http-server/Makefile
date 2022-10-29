CFLAGS = -g -Wall
CC = gcc

all: http_client

http_client: http_client.o
	$(CC) $(CFLAGS) http_client.o -o http_client

http_client.o: http_client.c http_client.h
	$(CC) $(CFLAGS) -c http_client.c -o http_client.o

clean:
	rm *.o
	rm ./http_client