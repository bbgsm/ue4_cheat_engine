
#include "TCPClient.h"

TCPClient::TCPClient(int tcp_fd) : tcp_fd(tcp_fd) {

}

TCPClient::TCPClient(string ip, int port) : ip(ip), port(port) {
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    servAddr.sin_port = htons(port);
    if (connect(tcp_fd, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1) {
        puts("connect  error!");
    } else {
        puts("connect success!");
    }
}

ssize_t TCPClient::send(const void *buff, int len, int flag) {
    return ::send(tcp_fd, buff, len, flag);
}

ssize_t TCPClient::recv(void *buff, int len, int flag) {
    return ::recv(tcp_fd, buff, len, flag);
}

ssize_t TCPClient::recvo(void *buff, size_t len, int flag) {
    auto *tempBuff = (unsigned char *) buff;
    int totalRecv = 0;
    size_t size = len;
    while (size > 0) {
        ssize_t i = ::recv(tcp_fd, &tempBuff[totalRecv], size, flag);
        if (i == 0) {
            return i;
        } else if (i == -1) {
            // 数据接收错误，可能客户端断开连接
            printf("error during recvall: %d", (int) i);
            return i;
        }
        totalRecv += i;
        size -= i;
    }
    return totalRecv;
}


ssize_t TCPClient::close() {
    return ::close(tcp_fd);
}



