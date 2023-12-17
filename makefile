CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lpthread -lsqlite3 -lssl -lcrypto
DATABASE_DIR = database
USER_DIR = user
TASK_QUEUE_DIR = task_queue
GAME_DIR = game
all: server client

server: server.c $(TASK_QUEUE_DIR)/queue.c $(USER_DIR)/user.c $(DATABASE_DIR)/database.c server.h $(USER_DIR)/user.h $(DATABASE_DIR)/database.h $(GAME_DIR)/game.c $(GAME_DIR)/game.h
	$(CC) $(CFLAGS) -o server server.c $(TASK_QUEUE_DIR)/queue.c $(USER_DIR)/user.c $(DATABASE_DIR)/database.c $(GAME_DIR)/game.c $(LIBS)

client: client.c 
	$(CC) $(CFLAGS) -o client client.c $(LIBS)

clean:
	rm -f server client
