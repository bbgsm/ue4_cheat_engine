
#ifndef EP_ENGINE_H
#define EP_ENGINE_H

#include <MemoryTools.h>
#include <GameTools.h>
#include <hmutex.h>
#include "SocketTools.h"




class Engine : public CallBack {
public:
    // so地址
    Addr baseAddr = 0;
    // 矩阵地址
    Addr MxAddr = 0;
    // 矩阵地址2
    Addr MxAddr1 = 0;
    // Gname地址
    Addr Gname = 0;
    // 世界地址
    Addr Uworld = 0;
    // Uleve地址
    Addr Uleve = 0;
    Addr UworldBase = 0;
    Addr ActorArr[1000] = {};

    int ActorCount = 0;

    bool isRun = false;

    int px = 2340 / 2;
    int py = 1080 / 2;
    // 自瞄横向调整
    int zmH = 0;
    // 自瞄纵向调整
    int zmV = 0;
    int fps = 50;
    // 屏幕准星x
    int dpx = 1170;
    // 屏幕准星y
    int dpy = 540;
    int zmRange = 200;
    int zmMode = 1;

    // 自瞄玩家距离
    int zmDistance = 9999999;


    // 自瞄选中
    int zmSelectType = 0;


    // 自己是否开镜
    bool isKj = false;
    // 自瞄选中
    bool isZmXz = false;
    // 自己是否开火
    bool isFire = false;
    bool isBone = false;
    // 视图是否已清理
    bool isClear = false;

    // 倒地不瞄
    bool isZmDowm = false;
    bool isVehicle = false;
    bool isMaterial = false;

    // 连贯自瞄
    bool isZmCoherent = true;

    float kj = 0;
    // 预判
    float predict = 0.6;
    float matrix[16] = {0}; // 矩阵数据

    //内存工具
    MemoryTools *memTools = NULL;
    //socket工具
    SocketTools *socTools = NULL;
    //游戏计算工具
    GameTools *gameTools = NULL;

    hmutex_t listlocker{};

    Engine();

    void call(int status);

    void CallBackData(DataDec *dataDec);

    static void *start(void *v);

    static void *filter(void *d);

    void keyVif();

    void getjz();

    int getData();
};


#endif




















