
#include "TCPServer.h"


TCPServer::TCPServer(int port) : port(port) {
    tcp_fd = socket(PF_INET, SOCK_STREAM, 0);
    sockaddr_in servAddr{};
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    if (::bind(tcp_fd, (sockaddr *) &servAddr, sizeof(servAddr)) == -1) {
        puts("bind() error!");
        return;
    }
    if (::listen(tcp_fd, 5) == -1) {
        puts("listen() error!");
        return;
    }
    puts("server create success!");
}

TCPClient *TCPServer::accept() {
    int newClient = ::accept(tcp_fd, nullptr, nullptr);
    if (-1 == newClient) {
        puts("acept error!");
        return nullptr;
    } else {
        puts("new client connect");
    }
    return new TCPClient(newClient);
}


ssize_t TCPServer::close() {
    return ::close(tcp_fd);
}

