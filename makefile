CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lpthread -lsqlite3 -lssl -lcrypto -lm
SRC_DIR = src
BIN_DIR = bin

DATABASE_DIR = $(SRC_DIR)/database
GAME_DIR = $(SRC_DIR)/game
LOG_DIR = $(SRC_DIR)/log
MESSAGE_DIR = $(SRC_DIR)/message
TASK_QUEUE_DIR = $(SRC_DIR)/task_queue
USER_DIR = $(SRC_DIR)/user

SERVER_OBJECTS = $(BIN_DIR)/database.o $(BIN_DIR)/game.o $(BIN_DIR)/log.o $(BIN_DIR)/queue.o $(BIN_DIR)/user.o

all: create_bin_dir server client

server: $(SERVER_OBJECTS) $(SRC_DIR)/server.c
	$(CC) $(CFLAGS) -o server $(SRC_DIR)/server.c $(SERVER_OBJECTS) $(LIBS)

client: $(SRC_DIR)/client.c
	$(CC) $(CFLAGS) -o client $(SRC_DIR)/client.c $(LIBS)

$(BIN_DIR)/database.o: $(DATABASE_DIR)/database.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR)/game.o: $(GAME_DIR)/game.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR)/log.o: $(LOG_DIR)/log.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR)/queue.o: $(TASK_QUEUE_DIR)/queue.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR)/user.o: $(USER_DIR)/user.c
	$(CC) $(CFLAGS) -c -o $@ $<

create_bin_dir:
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)/*.o server client
