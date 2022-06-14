
#include "UDPServer.h"


UDPServer::~UDPServer() {
    close();
}


UDPServer::UDPServer(int port) : port(port) {
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
        printf("creat socket fail\n");
        return;
    }
    sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;          // 使用IPV4协议
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;  // 绑定本地IP
    int ret = bind(udp_fd, (sockaddr *) &local_addr, sizeof local_addr);
    if (ret < 0) {
        printf("bind fail:\n");
        close();
        return;
    } else {
        printf("init UDPServer Success !\n");
    }
}

ssize_t UDPServer::send(CLIENT_ADDR *src_addr, const void *buff, size_t len, int flag) {
    return ::sendto(udp_fd, buff, len, flag, (sockaddr *) src_addr, addr_len);
}

ssize_t UDPServer::recv(void *buff, size_t len, int flag) {
    ssize_t i = ::recv(udp_fd, buff, len, flag);
    return i;
}

ssize_t UDPServer::recvo(void *buff, size_t len, int flag) {
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


ssize_t UDPServer::recv(CLIENT_ADDR *src_addr, void *buff, size_t len, int flag) {
    return ::recvfrom(udp_fd, buff, len, flag, (sockaddr *) src_addr, (socklen_t *) &addr_len);
}

ssize_t UDPServer::recvo(CLIENT_ADDR *src_addr, void *buff, size_t len, int flag) {
    auto *tempBuff = (unsigned char *) buff;
    int totalRecv = 0;
    size_t size = len;
    while (size > 0) {
        ssize_t i = recvfrom(udp_fd, &tempBuff[totalRecv], size, flag, (sockaddr *) src_addr, (socklen_t *) &addr_len);
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


ssize_t UDPServer::close() {
    return ::close(udp_fd);
}


