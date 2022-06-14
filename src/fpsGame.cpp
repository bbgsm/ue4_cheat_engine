#include "fpsGame.h"

#include "fpsGamePublic.h"
#include "TimeTools.h"
#include "SocketTools.h"
#include "GnameUtils.h"
#include "touch.h"
#include "requests.h"
#include "hmutex.h"
#include "abiUtil.h"

string LogBuff;
char buff[1024];

bool islog = false;
bool isSelfNamelog = true;

float zmx = 0;
float zmy = 0;
float zmSpeed = 10;
// 是否开启自瞄
bool isZm = false;
// touch是否已初始化
bool touchIsRun = false;
// 自瞄选中玩家地址
Addr ZmXzAddr = 0;
int scr_width = 2340;
int scr_height = 1080;
// 触摸开始坐标
int zmTouchStartX = 1400;
int zmTouchStartY = 600;
// 当前触摸位置
int touchX = zmTouchStartX;
int touchY = zmTouchStartY;


void addLog(const string &str) {
//    cout << str << endl;
    LogBuff += str;
    LogBuff += "\n";
}


void pushLog() {
    if (LogBuff.empty()) return;
    cout << LogBuff << endl;
}


Engine::Engine() {
    hmutex_init(&listlocker);
    //初始化Socket插件
    socTools = new SocketTools(1001, 1002, this);
    //开始连接通讯
    socTools->initConnect();
    socTools->initRecv();
}

// 蒙了 计算地图角度的
void Rotate(float x1, float y1, float &x2, float &y2, float ap) {
    x2 = x1 * cos(ap) - y1 * sin(ap);
    y2 = x1 * sin(ap) + y1 * cos(ap);
}

//回调函数
void Engine::call(int status) {
    //断开连接
    if (status == SOC_ALL_CLOSE) {
        pushLog();
        delete memTools;
        delete socTools;
        delete gameTools;
        isRun = false;
        exit(1);
    }
}



void *Engine::start(void *v) {
    auto *engine = (Engine *) v;
    engine->memTools = MemoryTools::MakePm("com.**.**");
    if (engine->memTools->getProcessPid() <= 0) {
        engine->socTools->sendCMD(D_MESSGE, "游戏未运行"); //发送进程不存在指令给app
        addLog("Game not runing");
        engine->call(SOC_ALL_CLOSE);
        return nullptr;
    }
    engine->memTools->initPageMap();          //初始化pagemap内存读取
    engine->gameTools = new GameTools(engine->memTools);
    // sleep(10);
    //获取基址
    engine->getjz();         //读取基址

    //读取数据主方法
    while (engine->getData()) {
        if (!engine->isClear) {
            addLog("clear");
            pushLog();
            engine->socTools->sendCMD(D_CLEAR);
            engine->isClear = true;
        }
        sleep(4);
    }
    return nullptr;
}


void Engine::CallBackData(DataDec *dataDec) {
    int cmd = dataDec->getCmd();
    if (cmd == START) {          //开始读取
        scr_width = dataDec->getInt();
        scr_height = dataDec->getInt();
        fps = dataDec->getInt();
        isBone = dataDec->getInt() == 1;
        isVehicle = dataDec->getInt() == 1;
        isMaterial = dataDec->getInt() == 1;
        zmRange = dataDec->getInt();
        zmMode = dataDec->getInt();
        zmSpeed = (float) dataDec->getInt();
        touchX = zmTouchStartX = dataDec->getInt();
        touchY = zmTouchStartY = scr_height - dataDec->getInt();
        zmH = dataDec->getInt();
        zmV = dataDec->getInt();
        zmSelectType = dataDec->getInt();
        isZmDowm = dataDec->getInt() == 0;
        isZmCoherent = dataDec->getInt() == 0;
        px = scr_width / 2;
        py = scr_height / 2;
        dpx = scr_width / 2;
        dpy = scr_height / 2;

        pthread_t pth;
        pthread_create(&pth, nullptr, start, this);

    } else if (cmd == STOP) {
        isRun = false;
    } else if (cmd == FPS) {
        fps = dataDec->getInt();
    } else if (cmd == IS_BONE) {
        isBone = dataDec->getInt();
    } else if (cmd == IS_VEHICLE) {
        isVehicle = dataDec->getInt();
    } else if (cmd == IS_MATERIAL) {
        isMaterial = dataDec->getInt();
    } else if (cmd == ZM) {
        isZm = dataDec->getInt() == 1;
        if (isZm) {
            if (!touchIsRun) {
                initTouch();
                touchIsRun = true;
            }
            addLog("开启自瞄");
        } else {
            addLog("关闭自瞄");
        }
    } else if (cmd == ZM_RANGR) {
        zmRange = dataDec->getInt();
    } else if (cmd == ZM_MODE) {
        zmMode = dataDec->getInt();
    } else if (cmd == ZM_SPEED) {
        zmSpeed = (float) dataDec->getInt();
    } else if (cmd == ZM_H) {
        zmH = dataDec->getInt();
    } else if (cmd == ZM_V) {
        zmV = dataDec->getInt();
    } else if (cmd == ZM_SELECT_TYPE) {
        zmSelectType = dataDec->getInt();
    } else if (cmd == IS_ZM_DOWM) {
        isZmDowm = dataDec->getInt() == 0;
    } else if (cmd == IS_ZM_COHERENT) {
        isZmCoherent = dataDec->getInt() == 0;
    } else if (cmd == ZM_TOUCH_POSITTON) {
        touchX = zmTouchStartX = dataDec->getInt();
        touchY = zmTouchStartY = scr_height - dataDec->getInt();
    }

}

//对象列表过滤
void *Engine::filter(void *d) {
    auto *engine = (Engine *) d;
    MemoryTools *memTools = engine->memTools;
    Addr TActorArr[1000];
    int TActorCount;
    while (engine->isRun) {
        // 数组地址
        Addr ActorArray = memTools->readAddr(engine->Uleve, 0x70);
        // 数组数量
        int count = memTools->readI(engine->Uleve, 0x74);
        TActorCount = 0;
        for (int i = 0; i < count; i++) {
            Addr Object = memTools->readAddr(ActorArray, 0x4 * i);
            if (Object == 0)
                continue;
            //过滤缺页(陷阱)内存
            if (memTools->isMemoryTrap(Object + 0x1000)) {
                if (islog) {
                    sprintf(buff, "E_Object:%lX", Object);
                    addLog(buff);
                }
                continue;
            }
            if (TActorCount > 1000) {
                TActorCount = 0;
            }
            TActorArr[TActorCount++] = Object;
        }

        //线程锁
        hmutex_lock(&engine->listlocker);
        memcpy(engine->ActorArr, TActorArr, TActorCount * 4);
        engine->ActorCount = TActorCount;
        hmutex_unlock(&engine->listlocker);

        sleep(1);
    }
    return nullptr;
}

void Engine::getjz() {
    baseAddr = memTools->BaseAddr("lib xxxx .so");
    // Gname
    Gname = memTools->readAddr(baseAddr + abiUtil::abi(0x7BB793C, 0xA9F7A20));
    UworldBase = baseAddr + abiUtil::abi(0x78D3FA4, 0xABC6498);

    Addr a = memTools->readAddr(baseAddr + abiUtil::abi(0x7D7C8B0, 0xABA0EB8));

    // 矩阵1 MxAddr = 矩阵位置
    MxAddr = memTools->readAddr(a + 0x20) + abiUtil::abi(0x200, 0x270);

    Addr a1 = memTools->readAddr(baseAddr + abiUtil::abi(0x7D80910, 0xABA5030));
    // 矩阵2,使用此矩阵的fov判断是否开镜 MxAddr + 0x320 = 矩阵位置
    MxAddr1 = memTools->readAddr(a1 + abiUtil::abi(0x68, 0x98));
}


int Engine::getData() {

    ZmXzAddr = 0;
    isKj = false;
    // 是否为自己
    int flag = 0;
    int selfId = 0;
    int selfStatus = 0;
    bool isSlef;
    int selfSc = false;
    float selfDirection = 0;

    // 玩家数量
    int heroCount;
    // 人机数量
    int botCount;

    float firstX = 0;
    float firstY = 0;

    zmDistance = 99999;
    ObjectData objectData;
    // 自瞄选中玩家二维坐标
    C2D zm2d{0, 0, 0, 0};
    // 自瞄选中玩家三维坐标
    C3D zm3d{0, 0, 0};
    // 对象坐标
    C3D ObjCoor{0, 0, 0};
    // 自己坐标
    C3D SlefCoor{0, 0, 0};
    // 数据打包工具类
    DataEnc *dataEnc = socTools->getDataEnc();
    // 读取世界地址
    Uworld = memTools->readAddr(UworldBase);
    // 读取对象列表
    Uleve = memTools->readAddr(Uworld, abiUtil::abi(0x20, 0x30));
    // 游戏是否正在运行
    char name[32];
    // 是否打印log
    islog = true;
    // 是否打印自己名称log
    isSelfNamelog = true;
    // 是否运行
    isRun = true;
    // 玩家列表读取线程
    /*   pthread_t pth;
       pthread_create(&pth, nullptr, filter, this);*/
    if (islog && !isClear) {
        addLog("===============================");
        char timeStr[40];
        TimeTools::getFormatTime(timeStr);
        sprintf(buff, "startTime:%s", timeStr);
        addLog(buff);

        sprintf(buff, "Uworld:%lX", Uworld);
        addLog(buff);

        sprintf(buff, "baseAddr:%lX", baseAddr);
        addLog(buff);

        sprintf(buff, "Uleve:%lX", Uleve);
        addLog(buff);

        sprintf(buff, "MxAddr:%lX", MxAddr);
        addLog(buff);

        sprintf(buff, "MxAddr1:%lX", MxAddr1);
        addLog(buff);

        sprintf(buff, "Gname:%lX", Gname);
        addLog(buff);
    }

    while (true) {
        mlong start = TimeTools::getCurrentTime();
        int play = memTools->readI(Uworld, abiUtil::abi(0x24, 0x38));
        if (play == 0) {
            // 游戏结束
            isRun = false;
            break;
        }
        //   hmutex_lock(&listlocker);
        // 矩阵读取
        kj = memTools->readF(MxAddr1 + abiUtil::abi(0x1E0, 0x260));    // 开镜判断
        // 通过视野判断是否开镜
        isKj = kj > 1.2F;
        // 人物数量置零
        heroCount = 0;
        botCount = 0;
        // 自瞄人物
        float nx, ny;
        bool zmFlag = false;
        //添加自定义配置文件(0：UTF-8 1:UTF-16LE)
        dataEnc->putInt(1);



        // 读取对象数组
        Addr ActorArray = memTools->readAddr(Uleve, abiUtil::abi(0x70, 0xA0));
        // 读取对象数量
        int count = memTools->readI(Uleve, abiUtil::abi(0x74, 0xA8));

        if (islog) {
            sprintf(buff, "ActorCount:%d", count);
            addLog(buff);
        }

        // 读取对象列表
        for (int i = 0; i < count; i++) {
            // 读取对象地址
            Addr Object = memTools->readAddr(ActorArray, abiUtil::abi(0x4, 0x8) * i);
            if (Object == 0) continue;
            // 过滤缺页(陷阱)内存
            if (memTools->isMemoryTrap(Object + 0x1000)) {
                if (islog) {
                    sprintf(buff, "E_Object:%lX", Object);
                    addLog(buff);
                }
                continue;
            }

            // Addr Object = ActorArr[i];
            int oid = memTools->readI(Object, abiUtil::abi(0x10, 0x18));
            memset(name, 0, 0x20);
            // 读取物品名称
            gameTools->getObjName(oid, Gname, 0x20, name);
            // 物品名称在地址中没有对其 需要往后移动2位
            memcpy(name, name + 2, 30);
            // 坐标地址
            Addr ActorPosition = memTools->readAddr(Object, abiUtil::abi(0x170, 0x1D8));
            // 坐标地址校验失败跳过读取
            if (!MemoryTools::isAddrValid(ActorPosition)) continue;
            // 判断是否是人物,否则为物品
            if (isplayer(name)) {


#if defined(__aarch64__)
                // 是否是真人
                // bool isReal = memTools->readI(Object, 0x920) == 1;
                bool isReal = memTools->readI(Object, 0x3C) == 16777486;
                if (!isReal) continue;
#endif

                // 读取人机  0:真人 1:人机
                objectData.isbot = memTools->readI(Object, abiUtil::abi(0x684, 0x8CC));
                // 实际血量比例
                objectData.health =
                        memTools->readF(Object, abiUtil::abi(0x894, 0xB98)) /
                        memTools->readF(Object, abiUtil::abi(0x898, 0xB9C)) *
                        100.0F;
                // 读取团队id
                objectData.teamId = memTools->readI(Object, abiUtil::abi(0x66C, 0x8B0));
                if (objectData.teamId < 1) continue;
                // 玩家状态 0:正常 1:倒下 4:死亡
                int status = memTools->readI(Object, abiUtil::abi(0x1DB8, 0x2510));
                if (status == 4) continue;
                // 手持
                objectData.scid = memTools->readI(Object, abiUtil::abi(0x1C7C, 0x2250)) != 0 ? -1 : 0;
                // 是否为自己 备用偏移 0x2460
                isSlef = memTools->readI(Object, abiUtil::abi(0x1714, 0x1CB0)) == 1;
                // 对象为自己读取自己的坐标并跳过
                if (isSlef) {
                    selfSc = objectData.scid == -1;
                    // 读取是否开火
                    isFire = memTools->readI(Object + abiUtil::abi(0xF04, 0x13C0)) == 1;
                    // 读取自己团队id
                    selfId = objectData.teamId;
                    selfDirection = memTools->readF(Object + abiUtil::abi(0xD0, 0x118));
                    if (selfDirection < 0.0F) {
                        selfDirection = selfDirection + 360.0F;
                    }
                    memTools->readV(&SlefCoor, 0xC, ActorPosition, abiUtil::abi(0x160, 0x170));
                    if (isSelfNamelog) {
                        int selfNameLen = 0;
                        UTF8 tempName[50];
                        memset(tempName, 0, 50);
                        UTF16 selfName[50];
                        // 读取自己昵称
                        gameTools->getPlayerName(Object + abiUtil::abi(0x628, 0x850), (char *) selfName, &selfNameLen);
                        // UTF16 转 UTF8
                        Utf16_To_Utf8(selfName, tempName, selfNameLen, lenientConversion);
                        selfName[selfNameLen] = '\0';
                        sprintf(buff, "selfName:%s", tempName);
                        addLog(buff);
                        isSelfNamelog = false;
                    }
                    // 跳过显示自己
                    continue;
                } else {
                    // 跳过显示队友
                    if (selfId == objectData.teamId) continue;
                    memTools->readV(&ObjCoor, 0xC, ActorPosition, abiUtil::abi(0x160, 0x170));
                    if (ObjCoor.x == 0.0F || ObjCoor.y == 0.0F || ObjCoor.z == 0) continue;
                    // 玩家距离
                    objectData.distance = (int) MemoryTools::Cal3Ddistance(SlefCoor, ObjCoor);
                  /*   if (objectData.distance < 150) {
                        // 计算地图
                        Rotate((SlefCoor.y - ObjCoor.y) / 80.0F, (SlefCoor.x - ObjCoor.x) / 80.0F, objectData.d3X,
                               objectData.d3Y,
                               selfDirection / 360.0F * M_PI * 2.0F);

                        objectData.d3X = objectData.d3X * -1;

                    } else {
                        objectData.d3X = 0;
                        objectData.d3Y = 0;
                    }*/

                }

                if (objectData.distance > 500) continue;

                if (objectData.isbot) {
                    botCount++;
                } else {
                    heroCount++;
                }
                // 读取矩阵
                memTools->readV(matrix, 4 * 16, MxAddr);
                // 3D坐标计算为屏幕2D坐标并且判断玩家是
                if (GameTools::get3Dto2D(objectData.position, matrix, (float) px, (float) py, ObjCoor) && isBone) {
                    //网格
                    Addr Mesh_Offset = memTools->readAddr(Object, abiUtil::abi(0x358, 0x470));
                    // 获取骨骼2D坐标
                    gameTools->Get_Bone_2D(objectData.bone2D, matrix, px, py, Mesh_Offset);
                } else {
                    resrtBone(objectData.bone2D);
                }


                // 读取玩家名称
                gameTools->getPlayerName(Object + abiUtil::abi(0x628, 0x850), objectData.nickName, &objectData.nicklen);
                // 对象类型
                objectData.type = 1;
                // 准星是否是否选中玩家
                objectData.isSelect = objectData.type == 1 && isSelect(dpx, dpy, objectData.position);

                if (isZm && selfSc && (isZmDowm || (!isZmDowm && status == 0)) &&
                    ((zmMode == 0 && isFire) || (zmMode == 1 && isKj) || (zmMode == 2 && isKj && isFire))) {
                    if (isZmXz) {
                        // 已选中玩家
                        if (ZmXzAddr == Object) {
                            // 设置选中类型
                            objectData.isSelect = 2;

                            // 计算玩家屏幕坐标 与准星的实际距离
                            float fx = objectData.bone2D.Pit.x - (float) dpx;
                            float fy = objectData.bone2D.Pit.y - (float) dpy;

                            flag++;
                            if (flag >= 10) {

                                C2D m2d{};
                                C2D m2d1{};

                                GameTools::get3Dto2D(m2d, matrix, (float) px, (float) py, ObjCoor);
                                GameTools::get3Dto2D(m2d1, matrix, (float) px, (float) py, zm3d);

                                firstX = (m2d.x - m2d1.x) * predict;
                                firstY = (m2d.y - m2d1.y) * predict;

                                flag = 0;
                                // 保存游戏世界3维坐标
                                zm3d = ObjCoor;
                            }

                            fx += firstX;
                            fy += firstY;

                            nx = fx + (float) zmH;

                            if (isFire) {
                                if (kj < 2.0F) {
                                    kj = kj * 1.5F;
                                }
                                // 倍镜压枪计算
                                float down = (20.0F * kj);
                                ny = fy + (float) zmV + down;
                            } else {
                                ny = fy + (float) zmV;
                            }

                            // 保存自瞄范围内最近玩家的屏幕坐标
                            zm2d = objectData.bone2D.Pit;
                            zmFlag = true;
                        }
                        // 选中玩家
                    } else if (((objectData.position.x + objectData.position.w >= (float) (dpx - zmRange))
                                && (objectData.position.x - objectData.position.w <= (float) (dpx + zmRange)))
                               && ((objectData.position.y + objectData.position.h >= (float) (dpy - zmRange))
                                   && (objectData.position.y - objectData.position.h <= (float) (dpy + zmRange)))) {

                        int jdl = MemoryTools::Cal2Ddistance(dpx, dpy, (int) objectData.position.x,
                                                             (int) objectData.position.y);
                        int jbl = MemoryTools::Cal2Ddistance(dpx, dpy, (int) zm2d.x, (int) zm2d.y);

                        // 如果未选中玩家直接选中
                        // 已经选中玩家就判断当前玩家是否匹配优先级比已选中的玩家优先级高
                        // 高就替换原来已选中的玩家

                        if (ZmXzAddr == 0 || (zmSelectType == 0 && jdl < jbl) ||
                            (zmSelectType == 1 && objectData.distance < zmDistance)) {

                            zm2d = objectData.bone2D.Pit;
                            zm3d = ObjCoor;

                            zmDistance = objectData.distance;

                            float fx = objectData.bone2D.Pit.x - (float) dpx;
                            float fy = objectData.bone2D.Pit.y - (float) dpy;

                            nx = fx + (float) zmH;
                            ny = fy + (float) zmV + 10;

                            firstX = 0;
                            firstY = 0;

                            ZmXzAddr = Object;
                            zmFlag = true;
                        }
                    }
                }
            } else {
                objectData.type = qlx(name);
                if (objectData.type == 0) continue;
                // 是否显示物资
                if (!isVehicle && m_state == 4) continue;
                // 是否显示载具
                if (!isMaterial && m_state < 4) continue;
                // 不显示手持
                if (m_state == 1) {
                    int issc = memTools->readI(Object, 0x7C);
                    if (issc != 0)
                        continue;
                }
                // 读取坐标
                memTools->readV(&ObjCoor, 0xC, ActorPosition, abiUtil::abi(0x160, 0x170));
                if (ObjCoor.x == 0 || ObjCoor.y == 0 || ObjCoor.z == 0) continue;
                // 计算距离
                objectData.distance = (int) MemoryTools::Cal3Ddistance(SlefCoor, ObjCoor);
                if (objectData.distance > 500) continue;
                // 读取矩阵
                memTools->readV(matrix, 4 * 16, MxAddr);
                // 3D坐标转换屏幕坐标
                if (!GameTools::get3Dto2D(objectData.position, matrix, (float) px, (float) py, ObjCoor)) {
                    continue;
                }
            }
            // 数据打包
            putSortData(dataEnc, objectData, isBone);
            // 显示玩家地址
//            sprintf(objectData.nickName, "%lX", Object);
            /*    sprintf(objectData.nickName, "%f %f %f", ObjCoor.x, ObjCoor.y, ObjCoor.z);*/
//            sprintf(objectData.nickName, "%s", name);
//            objectData.nicklen = strlen(objectData.nickName);
            // 昵称数据添加
            dataEnc->putStr(objectData.nickName, objectData.nicklen);
            socTools->addCrs();     // 增加数据数量
            reset(objectData);
        }
        //  hmutex_unlock(&listlocker);

        if (islog) {
            sprintf(buff, "heroCount:%d", heroCount);
            addLog(buff);
            sprintf(buff, "botCount:%d", botCount);
            addLog(buff);
            sprintf(buff, "crs:%d", socTools->getCrs());
            addLog(buff);
            addLog("-------------");
            islog = false;
        }

        if (isZm && zmFlag) {
            zmx = nx;
            zmy = ny;
            isZmXz = true;
        } else {
            isZmXz = false;
            ZmXzAddr = 0;
        }

        dataEnc->putInt(heroCount);
        dataEnc->putInt(botCount);

        int crs = socTools->getCrs();
        socTools->sendData(D_DATA);                         // 发送数据
        isClear = false;
        if (crs == 0) {
            sleep(2);
        } else {
            mlong end = TimeTools::getCurrentTime();
            //计算fps每帧延时
            fpsDelay(start, end, 50);
        }
    }
    return 1;
}


// 手指是否按下
bool isDown = false;
// 手指编号
int finger = 4;
// 曲线横向偏移
float h = 8;

// 重置触摸位置
void touchReset() {
    touchX = zmTouchStartX + (rand() % 200);
    touchY = zmTouchStartY + (rand() % 200);
}

//自瞄线程
void zmf(htimer_t *timer) {
    if (!isZm) {
        // 关闭自瞄线程
        hloop_stop(((hevent_t *) (timer))->loop);
        touchIsRun = false;
        return;
    }
    // 自瞄是否开启并且已选中玩家
    if (isZm && ((ZmXzAddr != 0))) {
        if (!isDown) {
            touchDown(touchY, touchX, finger);
            isDown = true;
            return;
        }

        // 判断触摸位置是否超过范围
        if (touchX <= 100 || touchX > (scr_width - 100) || touchY <= 100 || touchY > (scr_height - 100)) {
            touchUp(finger);
            isDown = false;
            touchReset();
            return;
        }

        // 曲线公式
        float xx = zmSpeed / 10 * sqrt(abs(zmx) - h);
        float yy = zmSpeed / 10 * sqrt(abs(zmy) - h);

        touchX += (int) xx * (zmx > 0 ? 1 : -1);
        touchY += (int) yy * (zmy > 0 ? -1 : 1);
        // 触摸滑动
        touchMove(touchY, touchX, finger);

    } else if (isDown) {
        // 触摸抬起
        touchUp(finger);
        isDown = false;
        // 触摸重置
        touchReset();
    } else {
        // 触摸重置
        touchReset();
    }
    zmx = 0;
    zmy = 0;
}


int main(int argv, char **argc) {
    printf("abiUtil::abi:%d\n", abiUtil::abi(32, 64));
    killInvalidSelf("IGameProtect");
    Engine engine;
}


















