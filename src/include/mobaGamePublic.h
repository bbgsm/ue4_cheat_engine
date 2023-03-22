
#ifndef WZ_CHEAT_WZ_H
#define WZ_CHEAT_WZ_H

#include <dirent.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include "requests.h"
#include "abiUtil.h"
#include <iostream>

#define P 1

using namespace std;
char *user;
char *imei;
char *buff = new char[1024];


#if P == 1
string logBuff;

void addLog(const string &str) {
//    printf("%s\n", str.c_str());
    logBuff += str;
    logBuff += "\n";
}

#endif

void pushLog() {
#if P == 1
    if (logBuff.empty()) return;
     cout << logBuff << endl;
#endif
}

struct HeroAdd {
    int id;
    int camp;
    Addr xyadd;             // 坐标地址
    Addr hpadd;             // 血量地址
};

struct BaseAddr {
    Addr UworldBase = 0;
    Addr GameStateBase = 0;
    Addr SoBaseAddr = 0;
    Addr MxBase = 0;
    Addr yeBase = 0;
};

struct Buffm {
    int bbTime;
    int brTime;
    int rbTime;
    int rrTime;
};

struct Hero {
    int id;
    int hp;
    int x;
    int y;
    C2D c2d;
    int dts;
    int dt;
    int camp;
};

struct GameMap {
    Buffm buff;
    Hero heros[20];
};

int px = 2248 / 2;
int py = 1080 / 2;

int mx = 2248;
int my = 1080;


bool buffDisply = true;               // 是否显示buff时间
bool cdDisply = true;                 // 是否显示玩家cd

mlong gp = -1;                      // 群组id
char *ss = new char[512];
char *is = nullptr;                    // 服务器IP
int pt = 0;                          // 服务器PORT
// 是否分享
int isShare = 0;
// 是否本地分享
int isLocalShare = 0;
// 本地分享IP
char *localIP;


bool get3Dto2D(C2D &c2d, const float *Matrix, const float px, const float py, const C3D &obj) {

    float Raito1 = obj.x * Matrix[3] + obj.y * Matrix[7] + obj.z * Matrix[11] + Matrix[15];
    if (Raito1 < 0.01) {
        c2d = {9999, 9999, 0, 0};
        return false;
    }

    c2d.x = px +
            (obj.x * Matrix[0] + obj.y * Matrix[4] + obj.z * Matrix[8] + Matrix[12]) / Raito1 * px;
    c2d.y = py -
            (obj.x * Matrix[1] + (obj.y) * Matrix[5] + obj.z * Matrix[9] + Matrix[13]) / Raito1 *
            py;
    c2d.w = py - (obj.x * Matrix[1] + (obj.y + 4.0) * Matrix[5] + obj.z * Matrix[9] + Matrix[13]) /
                 Raito1 * py;
    c2d.h = (c2d.y - c2d.w) / 2;

    return true;
}


string &replace_all(string &str, const string &old_value, const string &new_value) {
    while (true) {
        string::size_type pos(0);
        if ((pos = str.find(old_value)) != string::npos) {
            str.replace(pos, old_value.length(), new_value);
        } else { break; }
    }
    return str;
}

void getStbIP(char *ipBuf) {
    FILE *fstream = NULL;
    char temp_buff[1024];
    memset(temp_buff, 0, sizeof(temp_buff));
    if (nullptr ==
        (fstream = popen("ifconfig wlan0 | grep \"inet addr:\" | awk \'{print $2}\' | cut -c 6-",
                         "r"))) {
        snprintf(ipBuf, 18, "%s", "0.0.0.0");
        return;
    }
    if (nullptr != fgets(temp_buff, sizeof(temp_buff), fstream)) {
        snprintf(ipBuf, 18, "%s", temp_buff);
    } else {
        snprintf(ipBuf, 18, "%s", "0.0.0.0");
        pclose(fstream);
        return;
    }
    pclose(fstream);
}

void killInvalidSelf(const char *bm) {
    DIR *dir = NULL;
    dirent *ptr = NULL;
    FILE *fp = NULL;
    char filepath[256];            // 大小随意，能装下cmdline文件的路径即可
    char filetext[128];            // 大小随意，能装下要识别的命令行文本即可
    char buff[256];
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
                if (strstr(filetext, bm) != 0) {
                    int pid = atoi(ptr->d_name);
                    if (pid != getpid()) {
                        memset(buff, 0, 256);
                        sprintf(buff, "kill %d", pid);
                        system(buff);
                    }
                }
                fclose(fp);
            }
        }
    }
    closedir(dir);                // 关闭路径
}

bool vifNum(int ran, int val) {
    if (val != 0) {
        int y = ((ran / 2 + 38) * 4 | 6);
        if (val == y) {
            return true;
        }
    }
    return false;
}

bool is64() {
    return sizeof(Addr);
}

//读取基址
BaseAddr getBaseAddr(MemoryTools *memTools) {
    BaseAddr baseAddr;
    baseAddr.SoBaseAddr = memTools->BaseAddr(" ");
    baseAddr.MxBase = memTools->BaseAddr("libunity.so") +
                      abiUtil::abi(0x134E784, 0x1402AF0);                 //矩阵地址
    baseAddr.UworldBase = baseAddr.SoBaseAddr + abiUtil::abi(0x2E81E08, 0x40E2840);
    baseAddr.GameStateBase = baseAddr.SoBaseAddr + abiUtil::abi(0x309F7B8, 0x43006F8);
    baseAddr.yeBase = baseAddr.SoBaseAddr + abiUtil::abi(0x2C5E7F8, 0x3EB3080);

#if P == 1
    sprintf(buff, "SoBaseAddr:%lX ", baseAddr.SoBaseAddr);
    addLog(buff);
    sprintf(buff, "GameStateBase:%lX ", baseAddr.GameStateBase);
    addLog(buff);
    sprintf(buff, "UworldBase:%lX", baseAddr.UworldBase);
    addLog(buff);
    sprintf(buff, "mxBase:%lX", baseAddr.MxBase);
    addLog(buff);
#endif
    return baseAddr;
}


int getList(int mCamp, HeroAdd *heroAdd, Addr UworldBase, MemoryTools *memTools) {

    Addr u1 = memTools->readL(UworldBase);
    Addr u2 = memTools->readL(u1);
    Addr u3 = memTools->readL(u2, 0x338);
    Addr UWorld = memTools->readL(u3, abiUtil::abi(0x10, 0x18));
    int count = memTools->readI(u3, abiUtil::abi(0x24, 0x2C));

#if P == 1
    sprintf(buff, "heroBase:%lX", UWorld);
    addLog(buff);

    sprintf(buff, "count:%d", count);
    addLog(buff);
#endif

    int j = 0;
    // 读取保存玩家列表
    for (int i = 0; i < count; i++) {

        Addr heroAddr = memTools->readAddr(UWorld, i * 0x10);

        int nNum = memTools->readI(heroAddr, -8);          // 英雄id
        if (nNum < 1 || nNum > 10) {
            continue;
        }


#if P == 1
        sprintf(buff, "heroAddr:%lX", heroAddr);
        addLog(buff);
#endif

        if (heroAddr == 0) continue;

        int id = memTools->readI(heroAddr, 0x20);          // 英雄id
        Addr hd = memTools->readAddr(heroAddr + 0xD8);    // 血量地址

        if (hd == 0) {
            continue;
        }

        // 阵营
        int camp = memTools->readI(heroAddr, 0x2C);

        if (camp != 1 && camp != 2) continue;

        //坐标地址偏移大小过滤假人
        long f = memTools->readAddr(heroAddr, 0x148);
        long g = memTools->readAddr(f, 0x80);

        if (!MemoryTools::isAddrValid(f)) {
            continue;
        }

        if (memTools->isMemoryTrap(g)) {
            continue;
        }
#if P == 1
        sprintf(buff, "f:%lX", f);
        addLog(buff);
        sprintf(buff, "g:%lX", g);
        addLog(buff);
#endif

        if (llabs(f - g) >= 0x2000000) {
            //  socTools->sendCMD(D_MESSGE, "环境检查异常关闭辅助"); //发送进程不存在指令给app
            //  call(SOC_ALL_CLOSE);
            addLog("数据出现异常");
            continue;
        }

#if P == 1
        sprintf(buff, "id:%d xyadd:%lX (f - g):%llX camp:%d",
                id,  // id
                heroAddr,                               // 玩家地址
                llabs(f - g),                        // 坐标地址偏移
                camp);                                  // 阵营
        addLog(buff);
#endif

        if (mCamp == -1 || camp != mCamp) {
            heroAdd[j].id = id;          // 英雄id
            heroAdd[j].hpadd = hd;    // 血量地址
            heroAdd[j].xyadd = heroAddr;                                     // 坐标地址
            heroAdd[j].camp = camp;
            j++;
        }

    }
    return j;
}


void getBuffTime(int mCamp, Buffm &buffm, Addr yeAddr, MemoryTools *memTools) {
    int buffOff = abiUtil::abi(-0x318, -0x330);
    if (mCamp == 1) {
        buffm.bbTime = memTools->readI(yeAddr, 0);
        buffm.brTime = memTools->readI(yeAddr, buffOff * 1);
        buffm.rbTime = memTools->readI(yeAddr, buffOff * 2);
        buffm.rrTime = memTools->readI(yeAddr, buffOff * 3);
    } else {
        buffm.rbTime = memTools->readI(yeAddr, 0);
        buffm.rrTime = memTools->readI(yeAddr, buffOff * 1);
        buffm.bbTime = memTools->readI(yeAddr, buffOff * 2);
        buffm.brTime = memTools->readI(yeAddr, buffOff * 3);
    }
}

//内存工具
BaseAddr baseAddr;

class IGame {
public:
    class GCallback {
    public:
        virtual void IGameCall(int v) = 0;
    };

    MemoryTools *memTools = nullptr;
    BaseAddr baseAddr{};


    class GCallback *callback = nullptr;

    IGame(GCallback *callback) : callback(callback) {

    }

    bool initBase() {
        memTools = MemoryTools::MakePm("com.*****");
        //进程不存在
        if (memTools->getPid() <= 0) {
            return false;
        }
        memTools->initPageMap();
        //获取基址
        baseAddr = getBaseAddr(memTools);
        return true;
    }

    int mCamp = 0;
    Addr mxad = 0;
    Addr ye = 0;
    HeroAdd heroAdd[20];
    int heroCount = 0;
    int itime[20];
    C3D c3D{0, 0, 0};
    float Matrix[16] = {0};

    void init(bool isCp) {

#if P == 1
        char timeStr[40];
        TimeTools::getFormatTime(timeStr);
        sprintf(buff, "startTime:%s", timeStr);
        addLog(buff);
        sprintf(buff, "mxad:%lX", mxad);
        addLog(buff);
#endif


        memset(itime, 0, 4 * 20);
        // 读取矩阵地址
        Addr mx1 = memTools->readAddr(baseAddr.MxBase);
        mxad = memTools->readAddr(mx1 + abiUtil::abi(0x28, 0x50)) + abiUtil::abi(0xA4, 0xC0);
        // 判断阵营
        mCamp = memTools->readF(mxad) > 1 ? 1 : 2;

#if P == 1
        addLog("\n------------分割线-------------");
        sprintf(buff, "阵营:%s", mCamp == 1 ? "蓝方" : "红方");
        addLog(buff);
#endif

        // 读取玩家列表
        heroCount = getList(isCp ? mCamp : -1, heroAdd, baseAddr.UworldBase, memTools);

#if P == 1
        sprintf(buff, "英雄数量:%d", heroCount);
        addLog(buff);
#endif

        // buff 偏移
        int pis[] = {0x38, abiUtil::abi(0x10, 0x18), 0x30,
                     abiUtil::abi(0xA0, 0xA8)};         //buff偏移

        ye = memTools->getPointers(baseAddr.yeBase, 4, pis) - abiUtil::abi(0xFE0, 0x10B8);
#if P == 1
        sprintf(buff, "ye:%lX", ye);
        addLog(buff);
        pushLog();
#endif

    }

    bool isRunn() {
        return memTools->readZ(baseAddr.GameStateBase);
    }

    void getHeroData(Hero &hero, int index, bool isLine = false) {

        hero.id = heroAdd[index].id;
        hero.camp = heroAdd[index].camp;
        // 当前血量
        int hp = memTools->readI(heroAdd[index].hpadd, 0x78);
        // 最大血量
        int maxhp = memTools->readI(heroAdd[index].hpadd, 0x80);
        // 计算比例
        hero.hp = (hp * 50 / maxhp);
        //
        if (hero.hp <= 0 && hp > 0) hero.hp = 1;

        if (hero.hp > 0) {
            // 读取坐标地址
            Addr f = memTools->readL(heroAdd[index].xyadd, 0x148);
            Addr g = memTools->readL(f, 0x80);

            // 读取坐标
            int offset = memTools->readI(f, 0x98);
            auto x = (float) memTools->readI(g, offset == 0 ? 0 : 0xC);
            auto y = (float) memTools->readI(g, offset == 0 ? 0x8 : 0x14);

            hero.x = (int) (x * ((float) my * (mCamp == 1 ? 0.3148148f : -0.3148148f) / 2.0f / 50000.0f)) + 150;
            hero.y = (int) (y * ((float) my * (mCamp == 1 ? -0.3148148f : 0.3148148f) / 2.0f / 50000.0f)) + 150;

            if (isLine) {
                c3D.x = x * 0.001F;
                c3D.y = 0.0F;
                c3D.z = y * 0.001F;
                // 读取矩阵
                memTools->readV(Matrix, 16 * 4, mxad);
                // 计算屏幕坐标
                get3Dto2D(hero.c2d, Matrix, (float) px, (float) py, c3D);
            }
        }

        // 是否读取CD
        if (cdDisply) {
            // 读取CD
            Addr p1 = memTools->readAddr(heroAdd[index].xyadd + 0xC8);
            Addr dtime = memTools->readAddr(p1 + 0xD8);
            hero.dt = memTools->readI(dtime, -0x5B0) / 8192 / 1000;
            hero.dts = memTools->readI(dtime, 0x4B0) / 8192 / 1000;
            if (itime[index] == 0) {
                int hd = memTools->readI(dtime, 0x4B0 + 0x4);
                if (hd == 0) {
                    hero.dt = -1;
                } else {
                    itime[index] = 1;
                }
            }
        }
    }

    void getBuff(Buffm &buffm) {
        // 获取buff时间
        getBuffTime(mCamp, buffm, ye, memTools);
    }

    int getHeroCount() const {
        return heroCount;
    }

    int getCamp() {
        return mCamp;
    }

    bool GameIsRunn() {

    }

};

#endif
