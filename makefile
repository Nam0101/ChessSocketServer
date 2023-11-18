CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lprotobuf-c

all: server client

server: server.c authentication.pb-c.c
	$(CC) $(CFLAGS) -o server server.c authentication.pb-c.c $(LIBS)

client: client.c authentication.pb-c.c
	$(CC) $(CFLAGS) -o client client.c authentication.pb-c.c $(LIBS)

clean:
	rm -f server client
