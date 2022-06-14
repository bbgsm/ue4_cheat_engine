
#include <cstdio>
#include <cstdlib>
#include "TCPClient.h"

int main() {
    char buff[] = "Hello UDP";
    /*  UDPClient client("127.0.0.1", 10086);
     client.send(buff, strlen(buff));
     ssize_t len = client.recv(buff, 9);
     buff[len] = '\0';
     printf("%s\n", buff);*/

    TCPClient client("127.0.0.1", 10086);
    client.send(buff, strlen(buff));
    ssize_t len = client.recv(buff, 9);
    buff[len] = '\0';
    printf("%s\n", buff);
}
