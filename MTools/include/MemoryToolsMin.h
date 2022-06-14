#include "Type.h"

using namespace std;

#pragma once


class MemoryTools {

public:


    ~MemoryTools();

    //读取指定包名内存 (syscall) 需要root身份执行
    static MemoryTools *MakePmSysCall(const char *pm) {
        return new MemoryTools(pm);
    }

    MemoryTools(const char *packName);

    int readI(Addr addr, int offset = 0);       //读取int
    ulong readUL(Addr addr, int offset = 0);     //读取ulong
    Addr readAddr(Addr addr, int offset = 0);    //读取ulong
    Addr readP(Addr addr, int offset = 0);       //读取指针
    mlong readL(Addr addr, int offset = 0);     //读取long
    float readF(Addr addr, int offset = 0);     //读取float
    double readD(Addr addr, int offset = 0);    //读取double
    mbyte readB(Addr addr, int offset = 0);     //读取byte

    int readV(void *buff, int len, Addr addr, int offset = 0);  //读取指定大小的值

    int writeI(int value, Addr addr, int offset = 0);                //写入int
    int writeL(mlong value, Addr addr, int offset = 0);              //写入long
    int writeF(float value, Addr addr, int offset = 0);              //写入float
    int writeD(double value, Addr addr, int offset = 0);             //写入double
    int writeB(mbyte value, Addr addr, int offset = 0);              //写入byte
    int writeV(void *value, int len, Addr addr, int offset = 0);   //写入指定大小的值

    void setPID(int pid);
    void setPackName(char *pm);       //设置包名

    Addr BaseAddr(const char *modName);            // 获取基址 从maps中检索模块


    Addr getPointers(Addr addr, int p_size, int *offsets);

    static bool isAddrValid(Addr addr);

    static mlong getCurrentTime();           //获取系统时间
    static int getPID(const char *bm);       //获取pid

    static void sleep_ms(int ms);               //毫秒延时
    static void sleep_us(int us);               //微秒延时

    int getProcessPid();                // 获取已设置的进程pid
};