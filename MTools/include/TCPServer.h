

#ifndef EP_TCPSERVER_H
#define EP_TCPSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include "TCPClient.h"

class TCPServer {
private:
    int tcp_fd = -1;
    int port;
public:
    TCPServer(int port);

    TCPClient *accept();

    ssize_t close();
};


#endif //EP_TCPSERVER_H
