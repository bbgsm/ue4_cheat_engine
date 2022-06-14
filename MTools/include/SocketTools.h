
#ifndef WZ_CHEAT_TESTSOCKET_H
#define WZ_CHEAT_TESTSOCKET_H

#include "Type.h"
#include "ByteUtil.h"
#include "DataEnc.h"
#include "DataDec.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

enum SocToolStatus {
    SOC_TCP_CLOSE,
    SOC_UDP_CLOSE,
    SOC_ALL_CLOSE
};

class CallBack {
public:
    //回调函数
    virtual void call(int status) = 0;

    virtual void CallBackData(DataDec *dataRec) = 0;
};


class SocketTools {
public:
    std::function<void(int status)> call;
    std::function<void(DataDec *dataDec)> CallBackData;

    SocketTools();

    SocketTools(int tcp_port, int udp_port, CallBack *callBack);

    SocketTools(CallBack *callBack);

    ~SocketTools();

private:
    struct sockaddr_in addr;


    int DEFAULT_TCP_PORT = 9999;
    int DEFAULT_UDP_PORT = 8888;

    int SEND_BUFF_SIZE = 1024 * 1024;
    int RECV_BUFF_SIZE = 1024 * 1024;

    mbyte recvBuffer[1024 * 1024] = {0};              // 接收数据缓冲区1M
    mbyte sendBuffer[1024 * 1024] = {0};              // 发送数据缓冲区1M

    int recvLen = 0;
    int sendLen = 0;

    int client = -1;
    int udpfd = -1;                         //  客户端句柄

    int IPSC = 0;

    int sgp = -1;
    int crs = 0;

    int *UDP_POITS = NULL;
    char **UDP_IPS = NULL;

    char *DEFAULT_UDP_IP = new char[10]{"127.0.0.1"};   //UDP默认发送IP
    char *DEFAULT_TCP_IP = new char[10]{"127.0.0.1"};   //UDP默认发送IP

    DataDec *dataDec;
    DataEnc *dataEnc;

    CallBack *m_callback;

private:
    bool udp_send(void *data, size_t len);

    bool tcp_send(void *data, size_t len);

public:

    mlong TCPRecvice();

    int s_recv(mbyte *buff, int index, mlong len);

    void setUDPPort(int port);

    void setTCPPort(int port);

    void initRecv();

    void UDP_int();

    void TCP_init();

    void initConnect();

    void connectUDP();

    void connectTCP();

    void setUDPIP(const char *ip);

    void setUDPIP(int count, ...);

    void addUDPIP(const char *ip);

    void resetUDPIP();

    void setTCPIP(const char *ip);

    void setCallBack(CallBack *callBack);

    void setGroup(mlong m_gp);

    void closeTCP();

    void closeUDP();

    DataEnc *getDataEnc();

    DataDec *getDataDec();

    bool sendData();

    void closeAll();

    void sendCMD(int cmd);

    void sendCMD(int cmd, int val);

    void sendCMD(int cmd, mlong val);

    void sendCMD(int cmd, float val);

    void sendCMD(int cmd, double val);

    void sendCMD(int cmd, mbyte val);

    void sendCMD(int cmd, const char *str, int len);

    void sendCMD(int cmd, const char *str);

    void addCrs();

    int getCrs();

    void sendData(int cmd);

};


#endif //WZ_CHEAT_TESTSOCKET_H
