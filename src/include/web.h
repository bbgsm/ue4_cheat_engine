

#ifndef EW_WEB_H
#define EW_WEB_H


#include "HttpServer.h"
#include "hssl.h"

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "WebSocketServer.h"
#include "EventLoop.h"
#include "htime.h"
#include <thread>

using namespace std;
using namespace hv;




int radomInt(int min, int max) {
    int r = rand() % (max - min + 1) + min;
    return r;
}




#endif //EW_WEB_H
