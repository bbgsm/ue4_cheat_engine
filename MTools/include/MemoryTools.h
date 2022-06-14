#include "Type.h"
#include <list>

using namespace std;

#pragma once


class MemoryTools {
public:
    //数据类型
    enum Type {
        MEM_DWORD = 0,             //4位
        MEM_QWORD = 1,             //8位
        MEM_FLOAT = 2,             //float
        MEM_DOUBLE = 3,            //double
        MEM_BYTE = 4               //1位
    };

    //内存范围
    enum Range {
        ALL = 0x1,                           // 所有内存
        B_BAD = 0x2,                         // B内存
        C_ALLOC = 0x4,                       // Ca内存
        C_BSS = 0x8,                         // Cb内存
        C_DATA = 0x10,                       // Cd内存
        C_HEAP = 0x20,                       // Ch内存
        JAVA_HEAP = 0x40,                    // Jh内存
        A_ANONMYOUS = 0x80,                  // A内存
        CODE_SYSTEM = 0x100,                 // Xs内存
        STACK = 0x200,                       // S内存
        ASHMEM = 0x400                       // As内存
    };
private:
    int ISize = sizeof(int);
    int LSize = sizeof(mlong);
    int FSize = sizeof(float);
    int DSize = sizeof(double);
    int BSize = sizeof(mbyte);
    int ULSize = sizeof(ulong);

    FILE *pagemap_fd = nullptr;
    int pagemap_pfd;
    size_t page_size = 0;
    int GAME_PID = -1;                    // 全局应用pid
    const char *protext = "-/|\\";
    // 搜索数值区
    list<RADDR> *DataList = new list<RADDR>;
    // 内存范围区
    list<RADDR> *MapList = new list<RADDR>;

    int MSearchFloat(float from_value, float to_value);

    int MSearchDword(int from_value, int to_value);

    int MSearchQword(mlong from_value, mlong to_value);

    int MSearchDouble(double from_value, double to_value);

    int MSearchByte(mbyte from_value, mbyte to_value);

    int MOffset(const mbyte *from_value, const mbyte *to_value, long offset, int type, int len);

    int pr(void *buff, size_t len, Addr addr, int offset);

    int pw(void *buff, int len, ulong addr, off_t offset);

    void readmaps_all();

    void readmaps_bad();

    void readmaps_c_alloc();

    void readmaps_c_bss();

    void readmaps_c_data();

    void readmaps_c_heap();

    void readmaps_java_heap();

    void readmaps_a_anonmyous();

    void readmaps_code_system();

    void readmaps_stack();

    void readmaps_ashmem();

public:

    //读取自身内存 (memcpy) 不需要root身份执行
    static MemoryTools *MakeSELF() {
        return new MemoryTools();
    }

    //读取指定包名内存 (syscall) 需要root身份执行
    static MemoryTools *MakePm(const char *pm) {
        return new MemoryTools(pm);
    }

    //读取指定pid内存 (pread arm/arm64) 需要root身份执行
    static MemoryTools *MakePID(int pid) {
        return new MemoryTools(pid);
    }


    ~MemoryTools();

    MemoryTools();

    MemoryTools(int pid);

    MemoryTools(const char *packName);

    void setPID(int pid);

    void initPageMap();              //初始化内存页读取

    Addr readSafeAddr(Addr addr, int offset = 0);    //保护读取addr内存,如果缺页直接返回0 需要先执行初始化initPageMap()方法,

    int readI(Addr addr, int offset = 0);       //读取int
    bool readZ(Addr addr, int offset = 0);       //读取int
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
    int writeV(void *value, int len, Addr addr, int offset = 0);    //写入指定大小的值

    Addr BaseAddr(const char *modName);            // 获取基址 从maps中检索模块
    Addr SBaseAddr(const char *modName);           // 获取基址 从smaps中检索模块

    Addr getPointers(Addr addr, int p_size, int *offsets);

    int getPointersValue(Addr addr, void *buff, int len, int p_size, long *offsets);

    bool dumpInit(const char *filePath);

    MemoryTools &dumpMem(Addr startAddr, Addr endAddr);     //持久化内存空间数据 可以链式调用

    void dumpClose();

    Addr dumpMem(const char *dumpName, const char *filePath);//持久化内存空间数据

    static bool isAddrValid(Addr addr);


    static int getPID(const char *bm);       // 获取pid
    static int Cal2Ddistance(int px, int py, int x, int y); // 求2d距离
    static float Cal3Ddistance(float z_x, float z_y, float z_z, float obj_x, float obj_y, float obj_z); //求3d距离
    static float Cal3Ddistance(C3D &slf, C3D &obj);// 求3d距离


    static void log(const char *str);

    void setSearchRange(int type);                     // 设置搜索范围
    void addMapRang(Addr startAddr, Addr endAddr);     // 手动添加搜索范围

    int MemorySearch(const char *value, int TYPE);             // 类型搜索,这里value需要传入一个地址
    int MemoryOffset(const char *value, long offset, int type);              // 搜索偏移
    int RangeMemorySearch(const char *from_value, const char *to_value, Type type);    // 范围搜索
    int RangeMemoryOffset(const char *from_value, const char *to_value, long offset, Type type);    // 范围偏移
    void MemoryWrite(const char *value, long offset, Type type);          // 内存写入

    void Print();                    // 打印Res里面的内容
    void PrintMap();                    // 打印Res里面的内容
    void ClearResults();             // 清除链表,释放空间
    void ClearMap();                 // 清理map数据

    list<RADDR> *getResults();                 // 获取结果,返回头指针
    list<RADDR> *getMapResults();              // 获取搜map结果
    int getProcessPid() const;                // 获取已设置的进程pid
    int getResCount();                  // 获取搜索的数量

    bool isMemoryTrap(Addr address) const;    //判断内存是否缺页
};