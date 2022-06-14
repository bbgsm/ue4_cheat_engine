#include "MemoryToolsMin.h"
#include <cmath>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/syscall.h>
#include <fstream>
#include <iostream>
#include "requests.h"

using namespace std;


int GAME_PID = -1;                    // 全局应用pid


int ISize = sizeof(int);
int LSize = sizeof(mlong);
int FSize = sizeof(float);
int DSize = sizeof(double);
int BSize = sizeof(mbyte);
int ULSize = sizeof(ulong);
int ADDRSize = sizeof(Addr);


int pr(void *buff, size_t len, Addr addr, int offset) {
    memset(buff, 0, len);
    if (!MemoryTools::isAddrValid(addr)) return 0;
    Addr lastAddr = addr + offset;
    iovec iov_ReadBuffer{}, iov_ReadOffset{};
    iov_ReadBuffer.iov_base = buff;
    iov_ReadBuffer.iov_len = len;

    iov_ReadOffset.iov_base = (void *) lastAddr;
    iov_ReadOffset.iov_len = len;

    return (int) syscall(SYS_process_vm_readv, GAME_PID, &iov_ReadBuffer, 1, &iov_ReadOffset, 1, 0);
}

int pw(void *buff, int len, ulong addr, off_t offset) {
    memset(buff, 0, len);
    if (!MemoryTools::isAddrValid(addr)) return 0;
    iovec iov_WriteBuffer{}, iov_WriteOffset{};
    iov_WriteBuffer.iov_base = buff;
    iov_WriteBuffer.iov_len = len;

    iov_WriteOffset.iov_base = (void *) (addr + offset);
    iov_WriteOffset.iov_len = len;
    // 大小
    return syscall(SYS_process_vm_writev, GAME_PID, &iov_WriteBuffer, 1, &iov_WriteOffset, 1, 0);
}


MemoryTools::~MemoryTools() {

}


MemoryTools::MemoryTools(const char *pm) {
    GAME_PID = getPID(pm);
}


void MemoryTools::setPID(int pid) {
    GAME_PID = pid;
}

void MemoryTools::setPackName(char *pm) {
    GAME_PID = getPID(pm);
}


int MemoryTools::getPID(const char *bm) {
    int p = 0;
    DIR *dir = NULL;
    struct dirent *ptr = NULL;
    FILE *fp = NULL;
    char filepath[256];            // 大小随意，能装下cmdline文件的路径即可
    char filetext[128];            // 大小随意，能装下要识别的命令行文本即可
    dir = opendir("/proc");        // 打开路径
    if (NULL != dir) {
        while ((ptr = readdir(dir)) != NULL)    // 循环读取路径下的每一个文件/文件夹
        {
            // 如果读取到的是"."或者".."则跳过，读取到的不是文件夹名字也跳过
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                continue;
            if (ptr->d_type != DT_DIR)
                continue;
            sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);    // 生成要读取的文件的路径

            fp = fopen(filepath, "r");    // 打开文件
            if (NULL != fp) {

                fgets(filetext, sizeof(filetext), fp);    // 读取文件
                if (strcmp(filetext, bm) == 0) {
                    p = 1;
                    break;
                }
                fclose(fp);
            }
        }
    }
    if (p == 0) {
        return 0;
    }
    closedir(dir);                // 关闭路径
    return atoi(ptr->d_name);
}

Addr MemoryTools::BaseAddr(const char *modName) {
    FILE *fp;
    char lj[64], buff[256];
    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == NULL) {
        puts("分析失败");
        return 0;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (!feof(fp) && strstr(buff, modName)) {
            ulong start;
            ulong end;
            sscanf(buff, "%lx-%lx", &start, &end);
            return start;
            // 这里使用lx是为了能成功读取特别长的地址
        }
    }
    fclose(fp);                    // 关闭文件指针
    return 0;
}


Addr MemoryTools::getPointers(Addr addr, int p_size, int *offsets) {
    Addr temp = 0;
    readV(&temp, LSize, addr);
    for (int i = 0; i < p_size; i++) {
        readV(&temp, LSize, temp + offsets[i]);
    }
    return temp;
}


/* 毫秒级 延时 */
void MemoryTools::sleep_ms(int ms) {
    struct timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = ms * 1000;    //
    select(0, NULL, NULL, NULL, &delay);
}

/* 微妙级 延时 */
void MemoryTools::sleep_us(int us) {
    struct timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = us;
    select(0, NULL, NULL, NULL, &delay);
}


int MemoryTools::readI(Addr addr, int offset) {
    int temp = 0;
    pr(&temp, ISize, addr, offset);
    return temp;
}

ulong MemoryTools::readUL(Addr addr, int offset) {
    ulong temp = 0;
    pr(&temp, ULSize, addr, offset);
    return temp;
}

Addr MemoryTools::readAddr(Addr addr, int offset) {
    return readUL(addr, offset);
}


mlong MemoryTools::readL(Addr addr, int offset) {
    mlong temp = 0;
    pr(&temp, LSize, addr, offset);
    return temp;
}

float MemoryTools::readF(Addr addr, int offset) {
    float temp = 0;
    pr(&temp, FSize, addr, offset);
    return temp;
}

double MemoryTools::readD(Addr addr, int offset) {
    double temp = 0;
    pr(&temp, DSize, addr, offset);
    return temp;
}

mbyte MemoryTools::readB(Addr addr, int offset) {
    mbyte temp = 0;
    pr(&temp, BSize, addr, offset);
    return temp;
}

int MemoryTools::readV(void *buff, int len, Addr addr, int offset) {
    if (addr == 0) return 0;
    return pr(buff, len, addr, offset);
}


int MemoryTools::writeI(int value, Addr addr, int offset) {
    return pw(&value, ISize, addr, offset);
}

int MemoryTools::writeL(mlong value, Addr addr, int offset) {
    return pw(&value, LSize, addr, offset);
}

int MemoryTools::writeF(float value, Addr addr, int offset) {
    return pw(&value, FSize, addr, offset);
}

int MemoryTools::writeD(double value, Addr addr, int offset) {
    return pw(&value, DSize, addr, offset);
}

int MemoryTools::writeB(mbyte value, Addr addr, int offset) {
    return pw(&value, BSize, addr, offset);
}

int MemoryTools::writeV(void *value, int len, Addr addr, int offset) {
    return pw(value, len, addr, offset);
}


bool MemoryTools::isAddrValid(Addr addr) {
    if (addr < 0x20000000 || addr % 4 != 0) {
        return false;
    }
    return true;
}


//获取系统时间
mlong MemoryTools::getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int MemoryTools::getProcessPid() {
    return GAME_PID;
}




