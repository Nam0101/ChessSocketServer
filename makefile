CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lprotobuf-c -lpthread
all: server client

server: server.c queue.c
	$(CC) $(CFLAGS) -o server server.c queue.c $(LIBS)

client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LIBS)

clean:
	rm -f server client