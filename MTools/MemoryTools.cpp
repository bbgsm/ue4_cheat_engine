#include "MemoryTools.h"
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

#define PRINT 0          // 0:不打印日志 1:打印日志
#define READ_TYPE 0      // 0:process_vm_readv 1:memcpy read Self

#define PAGEMAP_ENTRY 8

using namespace std;


#if READ_TYPE == 0

// 读取pagemap转换物理地址判断是否是缺页内存
bool MemoryTools::isMemoryTrap(Addr addr) const {
    if (!MemoryTools::isAddrValid(addr) || pagemap_fd == nullptr) return true;
    Addr offset = addr / page_size * PAGEMAP_ENTRY;
    if (fseek(pagemap_fd, offset, SEEK_SET) != 0) {
        return true;
    }
    uint64_t page_frame_number = 0;
    fread(&page_frame_number, 1, PAGEMAP_ENTRY - 1, pagemap_fd);
    return page_frame_number <= 0;
}

/*bool MemoryTools::isMemoryTrap(Addr addr) const {
    Addr page_index = addr / page_size;
    Addr v_offset = page_index * sizeof(uint64_t);
    Addr page_offset = addr % page_size;
    uint64_t page_frame_number = 0;
    pread(pagemap_pfd, &page_frame_number, sizeof(uint64_t), v_offset);
    if ((((uint64_t) 1 << 63) & page_frame_number) != 0) {
        return false;
    }
    return true;
}*/

#else
// mincore 判断是否是缺页内存
bool MemoryTools::isMemoryTrap(Addr addr) {
    if (!MemoryTools::isAddrValid(addr)) return true;
    unsigned char vec = 0;
    Addr pageStart = addr & (~(page_size - 1u));
    syscall(__NR_mincore, (void *) pageStart, page_size, &vec);
    return vec != 1;
}
#endif

void MemoryTools::log(const char *str) {
//    printf("%s\n", str);
#if PRINT == 1
    ofstream file;
    file.open("/data/local/tmp/tm.txt", std::ios::app);
    if (!file.is_open()) {
    std::cout << "cannot open file" << std::endl;
        return;
    }
    file << str << endl;
#endif
}


int MemoryTools::pr(void *buff, size_t len, Addr addr, int offset) {
    memset(buff, 0, len);
    Addr lastAddr = addr + offset;
    if (MemoryTools::isMemoryTrap(lastAddr)) {
        //printf("缺页内存:%lX\n", lastAddr);
        return 0;
    }
#if READ_TYPE == 0
    iovec iov_ReadBuffer{}, iov_ReadOffset{};
    iov_ReadBuffer.iov_base = buff;
    iov_ReadBuffer.iov_len = len;
    iov_ReadOffset.iov_base = (void *) lastAddr;
    iov_ReadOffset.iov_len = len;
    return (int) syscall(SYS_process_vm_readv, GAME_PID, &iov_ReadBuffer, 1, &iov_ReadOffset, 1, 0);
#else
    if (memcpy(buff, (void *) lastAddr, len) != nullptr)
        return len;
#endif
}


int MemoryTools::pw(void *buff, int len, ulong addr, off_t offset) {
    Addr lastAddr = addr + offset;
    if (MemoryTools::isMemoryTrap(lastAddr)) {
        return 0;
    }
#if READ_TYPE == 0
    iovec iov_WriteBuffer{}, iov_WriteOffset{};
    iov_WriteBuffer.iov_base = buff;
    iov_WriteBuffer.iov_len = len;
    iov_WriteOffset.iov_base = (void *) (addr + offset);
    iov_WriteOffset.iov_len = len;
    // 大小
    return (int) syscall(SYS_process_vm_writev, GAME_PID, &iov_WriteBuffer, 1, &iov_WriteOffset, 1, 0);
#else
    if (memcpy((void *) (addr + offset), buff, len) != nullptr)
        return len;
#endif
    return 0;
}


void MemoryTools::initPageMap() {
    if (GAME_PID <= 0) return;
    char path_buf[0x100] = {};
    sprintf(path_buf, "/proc/%d/task/%d/pagemap", GAME_PID, GAME_PID);
    pagemap_fd = fopen(path_buf, "rb");
//    pagemap_pfd = open(path_buf, O_RDONLY);
}

MemoryTools::~MemoryTools() {
    if (DataList != nullptr) {
        delete DataList;
        DataList = nullptr;
    }
    if (MapList != nullptr) {
        delete MapList;
        MapList = nullptr;
    }
    if (pagemap_fd != nullptr) {
        fclose(pagemap_fd);
        pagemap_fd = nullptr;
    }
}


MemoryTools::MemoryTools() {
    GAME_PID = getpid();
    page_size = sysconf(_SC_PAGESIZE);
}

MemoryTools::MemoryTools(int pid) {
    GAME_PID = pid;
    page_size = sysconf(_SC_PAGESIZE);
}

MemoryTools::MemoryTools(const char *pm) {
    GAME_PID = getPID(pm);
    page_size = sysconf(_SC_PAGESIZE);
}

void MemoryTools::readmaps_all() {
    RADDR pmaps{0, 0};
    FILE *fp;
    char lj[64], buff[256];
    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp)) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);
        }
    }
    fclose(fp);                    // 关闭文件指针
}

void MemoryTools::readmaps_bad() {
    RADDR pmaps{0, 0};
    FILE *fp;
    char lj[64], buff[256];

    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp) && strstr(buff, "kgsl-3d0")) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);
        }
    }
    fclose(fp);                    // 关闭文件指针
}

void MemoryTools::readmaps_c_alloc() {
    RADDR pmaps{0, 0};
    FILE *fp;
    char lj[64], buff[256];

    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp) && strstr(buff, "[anon:libc_malloc]")) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);
        }
    }
    fclose(fp);                    // 关闭文件指针
}

void MemoryTools::readmaps_c_bss() {

    RADDR pmaps{0, 0};
    FILE *fp;
    char lj[64], buff[256];

    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp) && strstr(buff, "[anon:.bss]")) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);
        }
    }
    fclose(fp);                    // 关闭文件指针
}

void MemoryTools::readmaps_c_data() {
    RADDR pmaps{0, 0};
    FILE *fp;
    char lj[64], buff[256];
    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp) && strstr(buff, "/data/app/")) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);
        }
    }

    fclose(fp);                    // 关闭文件指针
}

void MemoryTools::readmaps_c_heap() {
    RADDR pmaps{0, 0};
    FILE *fp;
    char lj[64], buff[256];

    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp) && strstr(buff, "[heap]")) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);
        }
    }
    fclose(fp);                    // 关闭文件指针
}

void MemoryTools::readmaps_java_heap() {
    RADDR pmaps{0, 0};
    FILE *fp;
    char lj[64], buff[256];
    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");

    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp) && strstr(buff, "anon:dalvik-main")) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);
        }
    }
    fclose(fp);                    // 关闭文件指针
}

void MemoryTools::readmaps_a_anonmyous() {
    RADDR pmaps{0, 0};
    FILE *fp;
    char lj[64], buff[256];
    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp) && (strlen(buff) < 46)) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);
        }
    }
    fclose(fp);                    // 关闭文件指针
}


void MemoryTools::readmaps_code_system() {
    RADDR pmaps{0, 0};
    FILE *fp;
    int i = 0, flag = 1;
    char lj[64], buff[256];

    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp) && strstr(buff, "/system")) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);
        }
    }

    fclose(fp);                    // 关闭文件指针
}

void MemoryTools::readmaps_stack() {
    RADDR pmaps{0, 0};
    FILE *fp;
    char lj[64], buff[256];
    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp) && strstr(buff, "[stack]")) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);
        }
    }

    fclose(fp);                    // 关闭文件指针
}

void MemoryTools::readmaps_ashmem() {
    RADDR pmaps{0, 0};
    FILE *fp;
    char lj[64], buff[256];
    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        if (strstr(buff, "rw") != nullptr && !feof(fp) && strstr(buff, "/dev/ashmem/") &&
            !strstr(buff, "dalvik")) {
            sscanf(buff, "%lx-%lx", &pmaps.addr, &pmaps.taddr);
            MapList->push_back(pmaps);

        }
    }
    fclose(fp);                    // 关闭文件指针
}

void MemoryTools::setPID(int pid) {
    GAME_PID = pid;
}

int MemoryTools::getPID(const char *bm) {
    int p = 0;
    DIR *dir = nullptr;
    struct dirent *ptr = nullptr;
    FILE *fp = nullptr;
    char filepath[256];            // 大小随意，能装下cmdline文件的路径即可
    char filetext[128];            // 大小随意，能装下要识别的命令行文本即可
    dir = opendir("/proc");        // 打开路径
    if (nullptr != dir) {
        while ((ptr = readdir(dir)) != nullptr)    // 循环读取路径下的每一个文件/文件夹
        {
            // 如果读取到的是"."或者".."则跳过，读取到的不是文件夹名字也跳过
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                continue;
            if (ptr->d_type != DT_DIR)
                continue;
            sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);    // 生成要读取的文件的路径

            fp = fopen(filepath, "r");    // 打开文件
            if (nullptr != fp) {

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
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
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

Addr MemoryTools::SBaseAddr(const char *modName) {
    FILE *fp;
    char lj[64], buff[256];
    sprintf(lj, "/proc/%d/smaps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return 0;
    }
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行
        char *name = (char *) buff;
        if (!feof(fp) && name[8] == '-' && strstr(buff, modName)) {
            ulong start = 0;
            ulong end = 0;
            /* char *pre[10];
             ulong vl;
             char *time[20];
             int count;
             char mdname[256];*/
            //sscanf(buff, "%lx-%lx %s %lx %s %d %[^\n]", &start, &end, pre,&vl,time,&count,mdname);
            sscanf(buff, "%lx-%lx", &start, &end);
            return start;
        }
    }
    fclose(fp);                    // 关闭文件指针
    return 0;
}

Addr MemoryTools::getPointers(Addr addr, int p_size, int *offsets) {
    Addr temp = 0;
    readV(&temp, ULSize, addr);
    for (int i = 0; i < p_size; i++) {
        readV(&temp, ULSize, temp + offsets[i]);
    }
    return temp;
}

int MemoryTools::getPointersValue(Addr addr, void *buff, int len, int p_size, long *offsets) {
    Addr temp = 0;
    readV(&temp, LSize, addr);
    for (int i = 0; i < p_size; i++) {
        if (i == (p_size - 1)) {
            return readV(buff, len, temp + offsets[i]);
        } else {
            readV(&temp, LSize, temp + offsets[i]);
        }
    }
    return 0;
}

FILE *dumpfile = nullptr;

bool MemoryTools::dumpInit(const char *filePath) {
    dumpfile = fopen(filePath, "wb");
    if (dumpfile == nullptr) {

#if PRINT == 1
        puts("创建文件失败!");
#endif

        return false;
    }
    return true;
}

MemoryTools &MemoryTools::dumpMem(Addr startAddr, Addr endAddr) {
    if (endAddr - startAddr <= 0) {
        return *this;
    }
    long readOff = startAddr;
    char buff[1024];
    while (true) {
        if (endAddr - readOff > 1024) {
            readV(buff, 1024, readOff);
            fwrite(buff, 1024, 1, dumpfile);
            readOff += 1024;
        } else {
            readV(buff, (Addr) (endAddr - readOff), readOff);
            fwrite(buff, 1, (endAddr - readOff), dumpfile);
            break;
        }
    }
    return *this;
}

void MemoryTools::dumpClose() {
    fclose(dumpfile);                    // 关闭文件指针
}

ulong MemoryTools::dumpMem(const char *dumpName, const char *filePath) {
    FILE *fp;
    char lj[64], buff[256];
    int buffSize = 1024 * 1024;
    char fileBuff[buffSize];
    sprintf(lj, "/proc/%d/maps", GAME_PID);
    fp = fopen(lj, "r");
    if (fp == nullptr) {
#if PRINT == 1
        puts("分析失败");
#endif
        return 0;
    }
    FILE *p = fopen(filePath, "wb");
    ulong size = 0;
    while (!feof(fp)) {
        fgets(buff, sizeof(buff), fp);    // 读取一行

        ulong start = 0;
        ulong end = 0;
        char *pre[10];
        ulong vl;
        char *time[20];
        int count;
        char mdname[256];
        memset(mdname, 0, 256);
        sscanf(buff, "%lx-%lx %s %lx %s %d %[^\n]", &start, &end, pre, &vl, time, &count, mdname);
        if (strstr(mdname, dumpName)) {

            long readOff = start;
            char buff[1024];
            while (true) {
                if (end - readOff > buffSize) {
                    readV(fileBuff, buffSize, readOff);
                    fwrite(fileBuff, 1, buffSize, p);
                    readOff += buffSize;
                    size += buffSize;
                } else {
                    readV(fileBuff, (end - readOff), readOff);
                    fwrite(fileBuff, 1, (end - readOff), p);
                    size += end - readOff;
                    break;
                }
            }
            printf("write: %lx-%lx %s %lx %s %d %s -> %s\n", start, end,
                   pre, vl, time, count, mdname,
                   filePath);
        }
    }
    fclose(fp);                    // 关闭文件指针
    fclose(p);                    // 关闭文件指针
    return size;
}


int MemoryTools::Cal2Ddistance(int px, int py, int x, int y) {
    px = px - x;
    py = py - y;
    return (int) (sqrt(px * px + py * py));
}

float MemoryTools::Cal3Ddistance(float z_x, float z_y, float z_z, float obj_x, float obj_y, float obj_z) {
    return (sqrtf(powf(obj_x - z_x, 2.0F) + pow(obj_y - z_y, 2.0F) + powf(obj_z - z_z, 2.0F)) * 0.01F);
}

float MemoryTools::Cal3Ddistance(C3D &slf, C3D &obj) {
    return (sqrtf(powf(obj.x - slf.x, 2) + powf(obj.y - slf.y, 2) + powf(obj.z - slf.z, 2)) *
            0.01F);
}

int MemoryTools::readI(Addr addr, int offset) {
    int temp = 0;
    pr(&temp, ISize, addr, offset);
    return temp;
}

int16 MemoryTools::readI16(Addr addr, int offset) {
    int16 temp = 0;
    pr(&temp, I16LSize, addr, offset);
    return temp;
}

bool MemoryTools::readZ(Addr addr, int offset) {
    return readI(addr, offset) > 0;
}

ulong MemoryTools::readUL(Addr addr, int offset) {
    ulong temp = 0;
    pr(&temp, ULSize, addr, offset);
    return temp;
}

Addr MemoryTools::readAddr(Addr addr, int offset) {
    return readUL(addr, offset) & 0xFFFFFFFFFF;
}

Addr MemoryTools::readSafeAddr(Addr addr, int offset) {
    if (isMemoryTrap(addr + offset)) return 0;
    return readUL(addr, offset);
}

Addr MemoryTools::readP(Addr addr, int offset) {
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


void MemoryTools::setSearchRange(int type) {
    ClearMap();
    if (type & ALL) {
        readmaps_all();
        return;
    }
    if (type & B_BAD) {
        readmaps_bad();
    }
    if (type & C_ALLOC) {
        readmaps_c_alloc();
    }
    if (type & C_BSS) {
        readmaps_c_bss();
    }
    if (type & C_DATA) {
        readmaps_c_data();
    }
    if (type & C_HEAP) {
        readmaps_c_heap();
    }
    if (type & JAVA_HEAP) {
        readmaps_java_heap();
    }
    if (type & A_ANONMYOUS) {
        readmaps_a_anonmyous();
    }
    if (type & CODE_SYSTEM) {
        readmaps_code_system();
    }
    if (type & STACK) {
        readmaps_stack();
    }
    if (type & ASHMEM) {
        readmaps_ashmem();
    }
}


int MemoryTools::MSearchFloat(float from_value, float to_value) {
    int cs = 0;
    RADDR pmap{0, 0};
    int c;
    int buffSize = 0x1000 / FSize;
    auto *buff = new float[buffSize];    // 缓冲区
    float len = 0;
    float max = MapList->size();
    int position = 0;
    printf("mapSize:%d\n", (int) MapList->size());
    printf("\033[32;1m");
    // 迭代器
    list<RADDR>::iterator pmapsit;
    for (pmapsit = MapList->begin(); pmapsit != MapList->end(); ++pmapsit) {
        c = (int) (pmapsit->taddr - pmapsit->addr) / 0x1000;
        len++;
        position = (int) (len / max * 100.0F);
        printf("[%d%%][%c]\r", position, *(protext + (position % 4)));
        for (int j = 0; j < c; j += 1) {
            pr(buff, 0x1000, pmapsit->addr, j * 0x1000);
            for (int i = 0; i < buffSize; i += 1) {
                if (buff[i] >= from_value && buff[i] <= to_value) {
                    cs += 1;
                    pmap.addr = pmapsit->addr + (j * 0x1000) + (i * FSize);
                    DataList->push_back(pmap);
                }
            }
        }
    }
    printf("\033[37;1m");
    printf("\n");
    delete[] buff;
    return cs;
}


int MemoryTools::MSearchDword(int from_value, int to_value) {
    int cs = 0;
    RADDR pmap{0, 0};
    int c;
    int buffSize = 0x1000 / ISize;
    int *buff = new int[buffSize];    // 缓冲区
    float len = 0;
    float max = MapList->size();
    int position = 0;
    printf("mapSize:%d\n", (int) MapList->size());
    printf("\033[32;1m");
    // 迭代器
    list<RADDR>::iterator pmapsit;
    for (pmapsit = MapList->begin(); pmapsit != MapList->end(); ++pmapsit) {
        c = (int) (pmapsit->taddr - pmapsit->addr) / 0x1000;
        len++;
        position = (int) (len / max * 100.0F);
        printf("[%d%%][%c]\r", position, *(protext + (position % 4)));
        for (int j = 0; j < c; j += 1) {
            pr(buff, 0x1000, pmapsit->addr, j * 0x1000);
            for (int i = 0; i < buffSize; i += 1) {
                if (buff[i] >= from_value && buff[i] <= to_value) {
                    cs += 1;
                    pmap.addr = pmapsit->addr + (j * 0x1000) + (i * ISize);
                    DataList->push_back(pmap);
                }
            }

        }
    }
    printf("\033[37;1m");
    printf("\n");
    delete[] buff;
    return cs;
}

int MemoryTools::MSearchQword(mlong from_value, mlong to_value) {
    int cs = 0;
    RADDR pmap{0, 0};
    int c;
    int buffSize = 0x1000 / LSize;
    auto *buff = new mlong[buffSize];    // 缓冲区
    float len = 0;
    float max = MapList->size();
    int position = 0;
    printf("mapSize:%d\n", (int) MapList->size());
    printf("\033[32;1m");
    // 迭代器
    list<RADDR>::iterator pmapsit;
    for (pmapsit = MapList->begin(); pmapsit != MapList->end(); ++pmapsit) {
        c = (int) (pmapsit->taddr - pmapsit->addr) / 0x1000;
        len++;
        position = (int) (len / max * 100.0F);
        printf("[%d%%][%c]\r", position, *(protext + (position % 4)));
        for (int j = 0; j < c; j += 1) {
            pr(buff, 0x1000, pmapsit->addr, j * 0x1000);
            for (int i = 0; i < buffSize; i += 1) {
                if (buff[i] >= from_value && buff[i] <= to_value) {
                    cs += 1;
                    pmap.addr = pmapsit->addr + (j * 0x1000) + (i * LSize);
                    DataList->push_back(pmap);
                }
            }
        }
    }
    printf("\033[37;1m");
    printf("\n");
    delete[] buff;
    return cs;
}

int MemoryTools::MSearchDouble(double from_value, double to_value) {
    int cs = 0;
    RADDR pmap{0, 0};
    int c;
    int buffSize = 0x1000 / DSize;
    auto *buff = new double[buffSize];    // 缓冲区
    float len = 0;
    float max = MapList->size();
    int position = 0;
    printf("mapSize:%d\n", (int) MapList->size());
    printf("\033[32;1m");
    // 迭代器
    list<RADDR>::iterator pmapsit;
    for (pmapsit = MapList->begin(); pmapsit != MapList->end(); ++pmapsit) {
        c = (int) (pmapsit->taddr - pmapsit->addr) / 0x1000;
        len++;
        position = (int) (len / max * 100.0F);
        printf("[%d%%][%c]\r", position, *(protext + (position % 4)));
        for (int j = 0; j < c; j += 1) {
            pr(buff, 0x1000, pmapsit->addr, j * 0x1000);
            for (int i = 0; i < buffSize; i += 1) {
                if (buff[i] >= from_value && buff[i] <= to_value) {
                    cs += 1;
                    pmap.addr = pmapsit->addr + (j * 0x1000) + (i * DSize);
                    DataList->push_back(pmap);
                }
            }
        }
    }
    printf("\033[37;1m");
    printf("\n");
    delete[] buff;
    return cs;
}

int MemoryTools::MSearchByte(mbyte from_value, mbyte to_value) {
    int cs = 0;
    RADDR pmap{0, 0};
    int c;
    int buffSize = 0x1000 / BSize;
    auto *buff = new mbyte[buffSize];    // 缓冲区
    float len = 0;
    float max = MapList->size();
    int position = 0;
    printf("mapSize:%d\n", (int) MapList->size());
    printf("\033[32;1m");
    // 迭代器
    list<RADDR>::iterator pmapsit;
    for (pmapsit = MapList->begin(); pmapsit != MapList->end(); ++pmapsit) {
        c = (int) (pmapsit->taddr - pmapsit->addr) / 0x1000;
        len++;
        position = (int) (len / max * 100.0F);
        printf("[%d%%][%c]\r", position, *(protext + (position % 4)));
        for (int j = 0; j < c; j += 1) {
            pr(buff, 0x1000, pmapsit->addr, j * 0x1000);
            for (int i = 0; i < buffSize; i += 1) {
                if (buff[i] >= from_value && buff[i] <= to_value) {
                    cs += 1;
                    pmap.addr = pmapsit->addr + (j * 0x1000) + (i * BSize);
                    DataList->push_back(pmap);
                }
            }
        }
    }
    printf("\033[37;1m");
    printf("\n");
    delete[] buff;
    return cs;
}

int MemoryTools::MemorySearch(const char *value, TYPE type) {
    ClearResults();
    if (MapList->empty()) {
        return -1;
    }
    int cs = 0;
    if (type == MEM_DWORD) {
        int val = atoi(value);
        cs = MSearchDword(val, val);
    } else if (type == MEM_QWORD) {
        mlong val = atoll(value);
        cs = MSearchQword(val, val);
    } else if (type == MEM_FLOAT) {
        float val = atof(value);
        cs = MSearchFloat(val, val);
    } else if (type == MEM_DOUBLE) {
        double val = atof(value);
        cs = MSearchDouble(val, val);
    } else if (type == MEM_BYTE) {
        mbyte val = atoi(value);
        cs = MSearchByte(val, val);
    }
    return cs;
}


int
MemoryTools::RangeMemorySearch(const char *from_value, const char *to_value, Type type)    // 范围搜索
{
    ClearResults();
    int cs = 0;
    if (type == MEM_DWORD) {
        int fval = atoi(from_value);
        int tval = atoi(to_value);
        cs = MSearchDword(fval, tval);
    } else if (type == MEM_QWORD) {
        mlong fval = atoll(from_value);
        mlong tval = atoll(to_value);
        cs = MSearchQword(fval, tval);
    } else if (type == MEM_FLOAT) {
        float fval = atof(from_value);
        float tval = atof(to_value);
        cs = MSearchFloat(fval, tval);
    } else if (type == MEM_DOUBLE) {
        double fval = atof(from_value);
        double tval = atof(to_value);
        cs = MSearchDouble(fval, tval);
    } else if (type == MEM_BYTE) {
        mbyte fval = atof(from_value);
        mbyte tval = atof(to_value);
        cs = MSearchByte(fval, tval);
    }
    return cs;
}


int MemoryTools::MOffset(const mbyte *from_value, const mbyte *to_value, long offset, int type, int len) {

    int cs = 0;
    auto *offList = new list<RADDR>;
    RADDR maps{0, 0};
    mbyte buf[len];    // 缓冲区
    // 迭代器
    list<RADDR>::iterator pmapsit;
    for (pmapsit = DataList->begin(); pmapsit != DataList->end(); ++pmapsit) {
        pr(buf, len, pmapsit->addr, (int) offset);
        bool isMatch = false;

        if (type == MEM_DWORD) {
            int value = *(int *) buf;
            int from = *(int *) from_value;
            int to = *(int *) to_value;
            if (value >= from && value <= to) {
                isMatch = true;
            }
        } else if (type == MEM_QWORD) {
            mlong value = *(mlong *) buf;
            mlong from = *(mlong *) from_value;
            mlong to = *(mlong *) to_value;
            if (value >= from && value <= to) {
                isMatch = true;
            }
        } else if (type == MEM_FLOAT) {
            float value = *(float *) buf;
            float from = *(float *) from_value;
            float to = *(float *) to_value;
            if (value >= from && value <= to) {
                isMatch = true;
            }
        } else if (type == MEM_DOUBLE) {
            double value = *(double *) buf;
            double from = *(double *) from_value;
            double to = *(double *) to_value;
            if (value >= from && value <= to) {
                isMatch = true;
            }
        } else if (type == MEM_BYTE) {
            mbyte value = *buf;
            mbyte from = *from_value;
            mbyte to = *to_value;
            if (value >= from && value <= to) {
                isMatch = true;
            }
        }

        if (isMatch) {
            cs += 1;
            maps.addr = pmapsit->addr;
            offList->push_back(maps);
        }
    }
    delete DataList;
    DataList = offList;
    return cs;
}

int MemoryTools::MemoryOffset(const char *value, long offset, int type) {

    mbyte *buff = nullptr;
    int cs = 0;
    if (type == MEM_DWORD) {
        int val = atoi(value);
        buff = new mbyte[ISize];
        memcpy(buff, &val, ISize);
        cs = MOffset(buff, buff, offset, type, ISize);

    } else if (type == MEM_QWORD) {

        mlong val = atoll(value);
        buff = new mbyte[LSize];
        memcpy(buff, &val, LSize);
        cs = MOffset(buff, buff, offset, type, LSize);

    } else if (type == MEM_FLOAT) {

        float val = atof(value);
        buff = new mbyte[FSize];
        memcpy(buff, &val, FSize);
        cs = MOffset(buff, buff, offset, type, FSize);

    } else if (type == MEM_DOUBLE) {

        double val = atof(value);
        buff = new mbyte[DSize];
        memcpy(buff, &val, DSize);
        cs = MOffset(buff, buff, offset, type, DSize);

    } else if (type == MEM_BYTE) {
        mbyte val = atoi(value);
        buff = new mbyte[BSize];
        memcpy(buff, &val, BSize);
        cs = MOffset(buff, buff, offset, type, BSize);
    }
    delete[] buff;
    return cs;
}


int
MemoryTools::RangeMemoryOffset(const char *from_value, const char *to_value, long offset,
                               Type type)    // 范围偏移
{
    mbyte *fbuff = nullptr;
    mbyte *tbuff = nullptr;
    int cs = 0;
    if (type == MEM_DWORD) {

        int fval = atoi(from_value);
        int tval = atoi(to_value);
        fbuff = new mbyte[ISize];
        tbuff = new mbyte[ISize];

        memcpy(fbuff, &fval, ISize);
        memcpy(tbuff, &tval, ISize);

        cs = MOffset(fbuff, tbuff, offset, type, ISize);

    } else if (type == MEM_QWORD) {

        mlong fval = atoll(from_value);
        mlong tval = atoll(to_value);

        fbuff = new mbyte[LSize];
        tbuff = new mbyte[LSize];

        memcpy(fbuff, &fval, LSize);
        memcpy(tbuff, &tval, LSize);

        cs = MOffset(fbuff, tbuff, offset, type, LSize);


    } else if (type == MEM_FLOAT) {

        float fval = atof(from_value);
        float tval = atof(to_value);

        fbuff = new mbyte[FSize];
        tbuff = new mbyte[FSize];

        memcpy(fbuff, &fval, FSize);
        memcpy(tbuff, &tval, FSize);
        cs = MOffset(fbuff, tbuff, offset, type, FSize);

    } else if (type == MEM_DOUBLE) {

        double fval = atof(from_value);
        double tval = atof(to_value);

        fbuff = new mbyte[DSize];
        tbuff = new mbyte[DSize];

        memcpy(fbuff, &fval, DSize);
        memcpy(tbuff, &tval, DSize);
        cs = MOffset(fbuff, tbuff, offset, type, DSize);

    } else if (type == MEM_BYTE) {

        mbyte fval = atof(from_value);
        mbyte tval = atof(to_value);

        fbuff = new mbyte[BSize];
        tbuff = new mbyte[BSize];

        memcpy(fbuff, &fval, BSize);
        memcpy(tbuff, &tval, BSize);
        cs = MOffset(fbuff, tbuff, offset, type, BSize);

    }

    delete[] fbuff;
    delete[] tbuff;

    return cs;

}


void MemoryTools::MemoryWrite(const char *value, long offset, Type type) {

    mbyte *buff;
    int len = 0;

    if (type == MEM_DWORD) {

        int val = atoi(value);
        buff = new mbyte[ISize];
        memcpy(buff, &val, ISize);
        len = ISize;

    } else if (type == MEM_QWORD) {

        mlong val = atoll(value);
        buff = new mbyte[LSize];
        memcpy(buff, &val, LSize);
        len = LSize;

    } else if (type == MEM_FLOAT) {

        float val = atof(value);
        buff = new mbyte[FSize];
        memcpy(buff, &val, FSize);
        len = FSize;

    } else if (type == MEM_DOUBLE) {

        double val = atof(value);
        buff = new mbyte[DSize];
        memcpy(buff, &val, DSize);

        len = DSize;
    } else if (type == MEM_BYTE) {

        mbyte val = atoi(value);
        buff = new mbyte[BSize];
        memcpy(buff, &val, BSize);
        len = BSize;
    }
    // 迭代器
    list<RADDR>::iterator pmapsit;
    for (pmapsit = DataList->begin(); pmapsit != DataList->end(); ++pmapsit) {
        pw(&value, len, pmapsit->addr, offset);
    }
}


void MemoryTools::Print() {
    // 迭代器
    list<RADDR>::iterator pmapsit;
    for (pmapsit = DataList->begin(); pmapsit != DataList->end(); ++pmapsit) {
        printf("addr:0x%lX,taddr:0x%lX\n", pmapsit->addr, pmapsit->taddr);
    }
}

void MemoryTools::PrintMap() {
    // 迭代器
    list<RADDR>::iterator pmapsit;
    for (pmapsit = MapList->begin(); pmapsit != MapList->end(); ++pmapsit) {
        printf("addr:0x%lX,taddr:0x%lX\n", pmapsit->addr, pmapsit->taddr);
    }
}

void MemoryTools::ClearResults()                // 清空
{
    DataList->clear();
}

void MemoryTools::ClearMap()                // 清空
{
    MapList->clear();
}


list<RADDR> *MemoryTools::getResults()                // 获取搜索出的结果
{
    return DataList;
}

list<RADDR> *MemoryTools::getMapResults()                // 获取搜map结果
{
    return MapList;
}

int MemoryTools::getPid() const {
    return GAME_PID;
}

int MemoryTools::getResCount() {
    return DataList->size();
}

void MemoryTools::addMapRang(Addr startAddr, Addr endAddr) {
    RADDR pmaps{startAddr, endAddr};
    MapList->push_back(pmaps);
}




