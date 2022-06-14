
#ifndef EP_TCPCLIENT_H
#define EP_TCPCLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

using namespace std;

class TCPClient {
private:
    int tcp_fd;
    string ip;
    int port;
public:
    TCPClient(string ip, int port);

    TCPClient(int tcp_fd);

    ssize_t send(const void *buff, int len, int flag = 0);

    ssize_t recv(void *buff, int len, int flag = 0);

    ssize_t recvo(void *buff, size_t len, int flag = 0);

    ssize_t close();
};


#endif //EP_TCPCLIENT_H
