

#include <cstdio>
#include <cstdlib>
#include "TCPServer.h"
#include "TCPClient.h"

int main() {
    char buff[1024] = {};

    /* CLIENT_ADDR addr{};
     UDPServer server(10086);
     while (1) {
         int len = server.recv(&addr, buff, 1024);
         buff[len] = '\0';
         if (strcmp("exit", buff) == 0) {
             break;
         }
         server.send(&addr, "NB", 2);

         printf("%s\n", buff);
     }*/


    TCPServer server(10086);
    TCPClient *client = nullptr;
    while ((client = server.accept()) != nullptr) {
        while (true) {
            ssize_t len = client->recv(buff, 1024);
            if (len <= 0) {
                break;
            }
            buff[len] = '\0';
            if (strcmp("exit", buff) == 0) {
                break;
            }
            client->send("NB", 2);
            printf("%s\n", buff);
        }
        client->close();
    }

    return 0;
}
