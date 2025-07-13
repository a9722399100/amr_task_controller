CC = gcc
CFLAGS = -Wall -pthread
SRC = src
INC = include
BUILD = build

SERVER_SRC = $(SRC)/main.c $(SRC)/job_handler.c $(SRC)/task_queue.c $(SRC)/ui_ncurses.c
CLIENT_SRC = $(SRC)/client.c

all: $(BUILD)/server $(BUILD)/client

$(BUILD)/server: $(SERVER_SRC)
	$(CC) $(CFLAGS) -I$(INC) -o $@ $^ -lncurses

$(BUILD)/client: $(CLIENT_SRC)
	$(CC) $(CFLAGS) -I$(INC) -o $@ $^

clean:
	rm -f $(BUILD)/*
