

#include "UDPClient.h"


UDPClient::UDPClient(int port) {

}

UDPClient::UDPClient(string ip, int port) {
    int i = 1;
    socklen_t len = sizeof(i);
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &i, len);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

ssize_t UDPClient::sendto(char *ip, int port, const void *buff, int len, int flag) const {
    sockaddr_in temp_addr{};
    temp_addr.sin_port = htons(port);
    temp_addr.sin_addr.s_addr = inet_addr(ip);
    return ::sendto(udp_fd, buff, len, flag, (struct sockaddr *) &temp_addr, sizeof(temp_addr));
}


ssize_t UDPClient::send(const void *buff, int len, int flag) {
    return ::sendto(udp_fd, buff, len, flag, (struct sockaddr *) &addr, sizeof(addr));
}

ssize_t UDPClient::recv(void *buff, int len, int flag) {
    ssize_t i = ::recv(udp_fd, buff, len, flag);
    return i;
}

ssize_t UDPClient::recvo(void *buff, size_t len, int flag) {
    auto *tempBuff = (unsigned char *) buff;
    int totalRecv = 0;
    size_t size = len;
    while (size > 0) {
        ssize_t i = ::recv(udp_fd, &tempBuff[totalRecv], size, flag);
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


ssize_t UDPClient::close() {
    return ::close(udp_fd);
}
