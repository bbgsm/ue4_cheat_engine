
#include "SocketTools.h"


SocketTools::SocketTools() {
    dataDec = new DataDec(recvBuffer, RECV_BUFF_SIZE);
    dataEnc = new DataEnc(sendBuffer, SEND_BUFF_SIZE);
}

SocketTools::SocketTools(int tcp_port, int udp_port, CallBack *callBack) : m_callback(callBack) {
    dataDec = new DataDec(recvBuffer, RECV_BUFF_SIZE);
    dataEnc = new DataEnc(sendBuffer, SEND_BUFF_SIZE);
    DEFAULT_TCP_PORT = tcp_port;
    DEFAULT_UDP_PORT = udp_port;
}

SocketTools::SocketTools(CallBack *callBack) : m_callback(callBack) {
    dataDec = new DataDec(recvBuffer, RECV_BUFF_SIZE);
    dataEnc = new DataEnc(sendBuffer, SEND_BUFF_SIZE);
}

SocketTools::~SocketTools() {

    if (client > 0) {
        close(client);
    }
    if (udpfd > 0) {
        close(udpfd);
    }
    client = -1;
    udpfd = -1;

    if (DEFAULT_UDP_IP != nullptr) {
        delete[] DEFAULT_UDP_IP;
        DEFAULT_UDP_IP = nullptr;
    }
    if (DEFAULT_TCP_IP != nullptr) {
        delete[] DEFAULT_TCP_IP;
        DEFAULT_TCP_IP = nullptr;
    }
    if (UDP_IPS != nullptr) {
        for (int i = 0; i < IPSC; i++) {
            if (UDP_IPS[i] != nullptr) {
                delete[] UDP_IPS[i];
            }
        }
        delete[] UDP_IPS;
        DEFAULT_UDP_IP = nullptr;
    }
    delete dataDec;
}

int SocketTools::s_recv(mbyte *buff, int index, mlong len) {
    int totalRecv = index;
    size_t size = len;
    while (size > 0) {
        ssize_t i = recv(client, &buff[totalRecv], size, MSG_WAITALL);
        if (i == 0) {
            return i;
        } else if (i == -1) {
            // 数据接收错误，可能客户端断开连接
            printf("error during recvall: %d", (int) i);
            return i;
        }
        totalRecv += i;
        size -= i;
        recvLen += i;
    }
    return totalRecv;
}

bool SocketTools::udp_send(void *data, size_t len) {
    if (IPSC > 0) {
        if (UDP_IPS == nullptr) {
            return -1;
        }
        size_t ten = 0;
        for (int i = 0; i < IPSC; i++) {
            if (UDP_IPS[i] == nullptr) {
                continue;
            }
            addr.sin_port = htons(UDP_POITS[i]);
            addr.sin_addr.s_addr = inet_addr(UDP_IPS[i]);
            ten += sendto(udpfd, data, len, 0, (struct sockaddr *) &addr, sizeof(addr));
        }
        return ten == len;
    } else {
        addr.sin_port = htons(DEFAULT_UDP_PORT);
        addr.sin_addr.s_addr = inet_addr(DEFAULT_UDP_IP);
        return sendto(udpfd, data, len, 0, (struct sockaddr *) &addr, sizeof(addr)) == (ssize_t) len;
    }

}

// tcp发送数据
bool SocketTools::tcp_send(void *data, size_t len) {
    return send(client, data, len, 0) == (ssize_t) len;
}

void SocketTools::initConnect() {
    UDP_int();
    TCP_init();
}

void SocketTools::connectUDP() {
    UDP_int();
}

void SocketTools::connectTCP() {
    TCP_init();
}


void SocketTools::setTCPPort(int port) {
    DEFAULT_TCP_PORT = port;
}

void SocketTools::setUDPPort(int port) {
    DEFAULT_UDP_PORT = port;
}

void SocketTools::setUDPIP(int count, ...) {

    char **ips = new char *[count];
    int *ports = new int[count];
    char *po = new char[5];

    const char sptr = ':';

    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count; i++) {
        char *c = va_arg(ap, char*);
        int slen = strlen(c);

        int spi = 0;
        for (int j = 0; j < slen; ++j) {
            if (c[j] == sptr) {
                spi = j;
                break;
            }
        }
        if (!spi) return;
        ips[i] = new char[spi];

        for (int j = 0; j < spi; ++j) {
            ips[i][j] = c[j];
        }
        int b = 0;
        for (int j = spi + 1; j < slen; ++j) {
            po[b] = c[j];
            b++;
        }
        ports[i] = atoi(po);
    }
    delete[] po;

    if (UDP_IPS != nullptr) {
        for (int i = 0; i < IPSC; i++) {
            delete[] UDP_IPS[i];
        }
        delete[] UDP_IPS;
        UDP_IPS = nullptr;
    }
    if (UDP_POITS != nullptr) {
        delete[] UDP_POITS;
        UDP_POITS = nullptr;
    }

    UDP_IPS = ips;
    UDP_POITS = ports;
    IPSC = count;
    va_end(ap);  //用于清理

}

void SocketTools::addUDPIP(const char *ip) {
    char **ips = new char *[IPSC + 1];
    int *ports = new int[IPSC + 1];

    for (int j = 0; j < IPSC; ++j) {
        int len = strlen(UDP_IPS[j]);
        ips[j] = new char[len];
        for (int i = 0; i < len; i++) {
            ips[j][i] = UDP_IPS[j][i];
        }
        ports[j] = UDP_POITS[j];
    }

    const char sptr = ':';
    int slen = strlen(ip);
    int spi = 0;
    for (int j = 0; j < slen; ++j) {
        if (ip[j] == sptr) {
            spi = j;
            break;
        }
    }
    if (!spi) return;


    ips[IPSC] = new char[slen + 1];
    memset(ips[IPSC], 0, slen);
    for (int j = 0; j < spi; ++j) {
        ips[IPSC][j] = ip[j];
    }

    ips[IPSC][slen] = '\0';
    char *po = new char[5];
    int b = 0;
    for (int j = spi + 1; j < slen; ++j) {
        po[b] = ip[j];
        b++;
    }
    ports[IPSC] = atoi(po);
    delete[] po;

    if (UDP_IPS != nullptr) {
        for (int i = 0; i < IPSC; i++) {
            delete[] UDP_IPS[i];
        }
        delete[] UDP_IPS;
        UDP_IPS = nullptr;
    }
    if (UDP_POITS != nullptr) {
        delete[] UDP_POITS;
        UDP_POITS = nullptr;
    }

    UDP_IPS = ips;
    UDP_POITS = ports;
    IPSC++;
}


void SocketTools::resetUDPIP() {
    if (UDP_IPS != nullptr) {
        for (int i = 0; i < IPSC; i++) {
            delete[] UDP_IPS[i];
        }
        delete[] UDP_IPS;
        UDP_IPS = nullptr;
    }
    if (UDP_POITS != nullptr) {
        delete[] UDP_POITS;
        UDP_POITS = nullptr;
    }
    IPSC = 0;
}

void SocketTools::setUDPIP(const char *ip) {
    if (DEFAULT_UDP_IP != nullptr) {
        delete[] DEFAULT_UDP_IP;
        DEFAULT_UDP_IP = nullptr;
    }
    int slen = strlen(ip);
    const char sptr = ':';
    int spi = 0;
    for (int j = 0; j < slen; ++j) {
        if (ip[j] == sptr) {
            spi = j;
            break;
        }
    }
    if (spi <= 0) {
        DEFAULT_UDP_IP = new char[slen];
        strcpy(DEFAULT_UDP_IP, ip);
        return;
    } else {
        DEFAULT_UDP_IP = new char[spi];
        memcpy(DEFAULT_UDP_IP, ip, spi);
        char *po = new char[10];
        memcpy(po, ip + spi + 1, slen - spi);
        DEFAULT_UDP_PORT = atoi(po);
    }
}


void SocketTools::setTCPIP(const char *ip) {
    if (DEFAULT_TCP_IP != nullptr) {
        delete[] DEFAULT_TCP_IP;
        DEFAULT_TCP_IP = nullptr;
    }
    int slen = strlen(ip);
    const char sptr = ':';
    int spi = 0;
    for (int j = 0; j < slen; ++j) {
        if (ip[j] == sptr) {
            spi = j;
            break;
        }
    }
    if (spi <= 0) {
        DEFAULT_TCP_IP = new char[slen];
        strcpy(DEFAULT_TCP_IP, ip);
        return;
    } else {
        DEFAULT_TCP_IP = new char[spi];
        memcpy(DEFAULT_TCP_IP, ip, spi);
        char *po = new char[10];
        memcpy(po, ip + spi + 1, slen - spi);
        DEFAULT_TCP_PORT = atoi(po);
    }

}

void SocketTools::setCallBack(CallBack *callBack) {
    m_callback = callBack;
}


void SocketTools::closeTCP() {
    if (client > 0) {
        close(client);
    }
    client = -1;
    if (m_callback != nullptr) {
        m_callback->call(SOC_TCP_CLOSE);
    }
    if (call != nullptr) {
        call(SOC_TCP_CLOSE);
    }
}

void SocketTools::closeUDP() {
    if (udpfd > 0) {
        close(udpfd);
    }
    udpfd = -1;
    if (m_callback != nullptr) {
        m_callback->call(SOC_UDP_CLOSE);
    }
    if (call != nullptr) {
        call(SOC_TCP_CLOSE);
    }
}

void SocketTools::closeAll() {
    if (client > 0) {
        close(client);
    }
    if (udpfd > 0) {
        close(udpfd);
    }
    client = -1;
    udpfd = -1;
    if (m_callback != nullptr) {
        m_callback->call(SOC_ALL_CLOSE);
    }
    if (call != nullptr) {
        call(SOC_TCP_CLOSE);
    }
}

mlong SocketTools::TCPRecvice() {
    // 接收数据头
    if (!s_recv(recvBuffer, 0, DataDec::headerSize())) {
        closeAll();
        return -1;
    }
    mlong datalen = dataDec->getLength();
    // 数据包长度为零说明只有指令
    if (datalen == 0) {
        return 0;
    } else if (datalen < 0 || (datalen + DataDec::headerSize()) > RECV_BUFF_SIZE) {
        return -2;
    }
    //接收数据包
    if (!s_recv(recvBuffer, DataDec::headerSize(), datalen)) {
        closeAll();
        return -1;
    }
    return recvLen;
}


void SocketTools::initRecv() {
    while (true) {
        // 监听并接收数据
        mlong s = TCPRecvice();
        if (!s) {
            break;
        }
        dataDec->setData(recvBuffer, recvLen);
        if (m_callback != nullptr) m_callback->CallBackData(dataDec);
        if (CallBackData != nullptr) CallBackData(dataDec);

        // 清理接收的数据
        dataDec->reset();
        recvLen = 0;
    }
    closeAll();
}


void SocketTools::UDP_int() {
    int i = 1;
    socklen_t len = sizeof(i);
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(udpfd, SOL_SOCKET, SO_BROADCAST, &i, len);
    addr.sin_family = AF_INET;
}

void SocketTools::TCP_init() {
    sockaddr_in serverAddr{};
    if ((client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        closeAll();
        return;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_TCP_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(DEFAULT_TCP_IP);
    if (connect(client, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        closeAll();
        return;
    }
}


bool SocketTools::sendData() {
    dataEnc->setCount(crs);
    if (dataEnc->getDataLen() - DataEnc::headerSize() < 0) return false;
    if (sgp > 0) {
        dataEnc->putLong(sgp);
    }
    bool rel = udp_send(dataEnc->getData(), dataEnc->getDataLen());
    dataEnc->reset();
    crs = 0;
    return rel;
}

void SocketTools::sendData(int cmd) {
    dataEnc->setCmd(cmd);
    sendData();
}


void SocketTools::sendCMD(int cmd) {
    dataEnc->setCmd(cmd);
    dataEnc->setCount(0);
    sendData();
}


void SocketTools::sendCMD(int cmd, int val) {
    dataEnc->setCmd(cmd);
    dataEnc->setCount(0);
    dataEnc->putInt(val);
    sendData();
}

void SocketTools::sendCMD(int cmd, mlong val) {
    dataEnc->setCmd(cmd);
    dataEnc->setCount(0);
    dataEnc->putLong(val);
    sendData();
}

void SocketTools::sendCMD(int cmd, float val) {
    dataEnc->setCmd(cmd);
    dataEnc->setCount(0);
    dataEnc->putFloat(val);
    sendData();
}

void SocketTools::sendCMD(int cmd, double val) {
    dataEnc->setCmd(cmd);
    dataEnc->setCount(0);
    dataEnc->putDouble(val);
    sendData();
}

void SocketTools::sendCMD(int cmd, mbyte val) {
    dataEnc->setCmd(cmd);
    dataEnc->setCount(0);
    dataEnc->putByte(val);
    sendData();
}

void SocketTools::sendCMD(int cmd, const char *str, int len) {
    dataEnc->reset();
    dataEnc->setCmd(cmd);
    dataEnc->setCount(0);
    dataEnc->putStr(str, len);
    sendData();
}

void SocketTools::sendCMD(int cmd, const char *str) {
    dataEnc->reset();
    dataEnc->setCmd(cmd);
    dataEnc->setCount(0);
    dataEnc->putStr(str, strlen(str));
    dataEnc->putInt(0);
    sendData();
}

void SocketTools::setGroup(mlong m_gp) {
    sgp = m_gp;
}

DataEnc *SocketTools::getDataEnc() {
    return dataEnc;
}

DataDec *SocketTools::getDataDec() {
    return dataDec;
}

void SocketTools::addCrs() {
    crs++;
}

int SocketTools::getCrs() {
    return crs;
}



