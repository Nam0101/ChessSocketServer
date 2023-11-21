CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lprotobuf-c -lpthread
all: server 

server: server.c authentication.pb-c.c
	$(CC) $(CFLAGS) -o server server.c authentication.pb-c.c $(LIBS)

# authentication:
	protoc-c --c_out=. authentication.proto
clean:
	rm -f server 
