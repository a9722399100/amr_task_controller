#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("請輸入任務參數，例如：\n");
        printf("./build/client \"REQUEST job=sample_collection; source=qc_node_3\"\n");
        return 1;
    }

    // 組合輸入字串
    char input[BUFFER_SIZE];
    snprintf(input, sizeof(input), "%s", argv[1]);

    // 建立 socket 並連接 server
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    // 傳送任務
    send(sock, input, strlen(input), 0);

    // 接收伺服器訊息
    char buffer[BUFFER_SIZE];
    while (recv(sock, buffer, sizeof(buffer) - 1, 0) > 0) {
        buffer[strcspn(buffer, "\n")] = 0;
        printf("[Server] %s\n", buffer);
    }

    close(sock);
    return 0;
}

