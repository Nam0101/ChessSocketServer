CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lprotobuf-c -lpthread -lsqlite3
PROTO_DIR = .
PROTO_FILES = $(PROTO_DIR)/message.pb-c.c

all: server client

server: server.c queue.c $(PROTO_FILES)
	$(CC) $(CFLAGS) -o server server.c queue.c $(PROTO_FILES) $(LIBS)

client: client.c $(PROTO_FILES)
	$(CC) $(CFLAGS) -o client client.c $(PROTO_FILES) $(LIBS)

$(PROTO_DIR)/message.pb-c.c:
	protoc-c --c_out=$(PROTO_DIR) $(PROTO_DIR)/message.proto

clean:
	rm -f server client $(PROTO_FILES)
