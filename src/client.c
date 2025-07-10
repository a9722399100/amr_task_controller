#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("使用方式：%s \"<job_description>\" <priority> [server_ip]\n", argv[0]);
        printf("範例：%s \"REQUEST job=material_delivery; from=storage; to=station1\" 2 [192.168.x.x]\n", argv[0]);
        return 1;
    }

    char input[BUFFER_SIZE];
    snprintf(input, sizeof(input), "%s; priority=%s", argv[1], argv[2]);

    const char* server_ip = (argc >= 4) ? argv[3] : "127.0.0.1";  // 預設為本機

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, server_ip, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    send(sock, input, strlen(input), 0);

    char buffer[BUFFER_SIZE];
    while (recv(sock, buffer, sizeof(buffer) - 1, 0) > 0) {
        buffer[strcspn(buffer, "\n")] = 0;
        printf("[Server] %s\n", buffer);
    }

    close(sock);
    return 0;
}

