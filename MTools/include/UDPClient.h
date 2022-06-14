
#ifndef EP_UDPCLIENT_H
#define EP_UDPCLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

using namespace std;


class UDPClient {
private:
    int udp_fd = -1;
    sockaddr_in addr{};

public:
    UDPClient(int port);

    UDPClient(string ip, int port);

    ssize_t sendto(char *ip, int port, const void *buff, int len, int flag = 0) const;

    ssize_t send(const void *buff, int len, int flag = 0);

    ssize_t recv(void *buff, int len, int flag = 0);

    ssize_t recvo(void *buff, size_t len, int flag = 0);

    ssize_t close();
};


#endif //EP_UDPCLIENT_H
