CC = gcc
CFLAGS = -Wall -pthread
SRC = src
INC = include
BUILD = build

# 預設目標：編譯 server 和 client
all: $(BUILD)/server $(BUILD)/client

# 編譯 server：使用 main.c 和 job_handler.c
$(BUILD)/server: $(SRC)/main.c $(SRC)/job_handler.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -I$(INC) -o $@ $^

# 編譯 client：單一檔案 client.c
$(BUILD)/client: $(SRC)/client.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -I$(INC) -o $@ $^

# 清除編譯輸出
clean:
	rm -f $(BUILD)/*

