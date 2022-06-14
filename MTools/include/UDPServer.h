
#ifndef EP_UDPSERVER_H
#define EP_UDPSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

#define CLIENT_ADDR sockaddr_in

class UDPServer {
private:
    int udp_fd;
    int port;
    int addr_len = sizeof(sockaddr_in);
public:
    UDPServer(int port);

    ~UDPServer();

    ssize_t send(sockaddr_in *src_addr, const void *buff, size_t len, int flag = 0);

    ssize_t recv(void *buff, size_t len, int flag = 0);

    ssize_t recvo(void *buff, size_t len, int flag = 0);

    ssize_t recv(sockaddr_in *src_addr, void *buff, size_t len, int flag = 0);

    ssize_t recvo(sockaddr_in *src_addr, void *buff, size_t len, int flag = 0);


    ssize_t close();
};


#endif //EP_UDPSERVER_H
