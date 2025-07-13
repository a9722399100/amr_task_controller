CC = gcc
CFLAGS = -Wall -pthread
SRC = src
INC = include
BUILD = build

SERVER_SRC = $(SRC)/main.c $(SRC)/job_handler.c $(SRC)/task_queue.c $(SRC)/ui_ncurses.c
CLIENT_SRC = $(SRC)/client.c
LIBS = -lncurses

all: $(BUILD)/server $(BUILD)/client

$(BUILD)/server: $(SERVER_SRC)
	$(CC) $(CFLAGS) -I$(INC) -o $@ $^ $(LIBS)

$(BUILD)/client: $(CLIENT_SRC)
	$(CC) $(CFLAGS) -I$(INC) -o $@ $^

clean:
	rm -f $(BUILD)/*

