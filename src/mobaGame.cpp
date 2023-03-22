/**
 * moba手游读取
 */

#include <thread>
#include <cstdlib>
#include <unistd.h>
#include "MemoryTools.h"
#include "TimeTools.h"
#include "mobaGamePublic.h"
#include "rxlua.h"
#include "SocketTools.h"
#include "web.h"
#include "hmutex.h"

using namespace std;

bool localIsRun = false;    // 本地绘制是否启动
bool webIsRun = false;      // web绘制是否启动
bool baseIsInit = false;    // 基址是否初始化
bool isWeb = false;         // 是否使用web绘制

hmutex_t listlocker{};
websocket_server_t sserver;
http_server_t wserver;

void *websocketServer(void *v);

//视图清理标识 暂时解决没绘制会经常闪烁的问题
bool isClear = false;
// CMD命令
enum CMD {
    START = 10001,                        // 开始 搜索内存
    STOP = 10002,                         // 结束
    BUFF_DISPLY = 10003,                  // buff显示设置
    CD_DISPLAY = 10004,                   // CD显示
    WEB_SERVICE = 10006,                    // CD显示
    INIT = 10007                           // CD显示
};

// 数据交互命令
enum DATA_INTERACTION {
    D_DATA = 11000,                        // 游戏数据
    D_GAME_VOER = 11001,                   // 游戏结束
    D_BLUE = 11002,                        // 我方蓝方
    D_RED = 11003,                         // 我方红方
    D_TEST = 11004,                        // Test
    D_CLEAR = 11005,                       // 清理视图
    D_HEAHER = 11006,                      // 玩家头像
    D_MESSGE = 11007,                      // 消息
    D_LOG = 11008,                         // 日志
    D_PROISNOT = 11014,                    // 游戏未运行
    D_SHARE = 11015,                        // 游戏进程未找到
    D_FLAT_VIS = 11016
};

IGame *iGame = nullptr;
//socket工具
SocketTools *socTools = nullptr;

class Engine : public CallBack {
public:
    void call(int status) override {
        if (status == SOC_ALL_CLOSE) {
            pushLog();
            localIsRun = false;
            exit(1);
        }
    }

    void closeAll() {
        if (user != nullptr) {
            delete user;
            user = nullptr;
        }
        if (imei != nullptr) {
            delete imei;
            imei = nullptr;
        }
        if (ss != nullptr) {
            delete[] ss;
            ss = nullptr;
        }
        if (is != nullptr) {
            delete is;
            is = nullptr;
        }
        socTools->closeAll();
    }

    Engine() {
        //初始化Socket插件
        socTools = new SocketTools(this);
        socTools->setTCPPort(10508);
        //开始连接通讯
        socTools->initConnect();
        //开始监听
        socTools->initRecv();
    }

    ~Engine() {
        closeAll();
    }


    static void *vm(void *d) {
        hmutex_lock(&listlocker);
        auto *engine = (Engine *) d;
        if (!baseIsInit) {
            iGame = new IGame(nullptr);
            bool initBase = iGame->initBase();
            //进程不存在
            if (!initBase) {
                //printf("进程不存在\n");
                socTools->sendCMD(D_MESSGE, "游戏未运行"); //发送进程不存在指令给app
                addLog("Game not runing");
                engine->closeAll();
                return nullptr;
            }
        }
        localIsRun = true;
        while (localIsRun) {
            engine->getData();   //读取数据主方法
            if (!isClear) {
                socTools->sendCMD(D_CLEAR);
                isClear = true;
            }
            if (localIsRun) {
                sleep(4);
            } else {
                break;
            }

        }
        hmutex_unlock(&listlocker);
        return nullptr;
    }



    void callFun() {
        pthread_t engThread;
        if (isWeb == 1) {
            localIsRun = false;
            pthread_create(&engThread, nullptr, websocketServer, this);
        } else {
            webIsRun = false;
            pthread_create(&engThread, nullptr, vm, this);
        }
    }

    //监听数据处理
    void CallBackData(DataDec *dataDec) override {
        int cmd = dataDec->getCmd();

#if P == 1
        sprintf(buff, "cmd:%d", cmd);
        addLog(buff);
#endif

        if (cmd == INIT) {
            mx = dataDec->getInt();
            my = dataDec->getInt();
            user = dataDec->getStr();
            imei = dataDec->getStr();
            is = dataDec->getStr();              //获取ip
            pt = dataDec->getInt();              //获取端口

        } else if (cmd == START) {

            buffDisply = dataDec->getBool();      //buff
            cdDisply = dataDec->getBool();        //技能CD
            isWeb = dataDec->getBool();           //
            isShare = dataDec->getBool();         //技能CD
            isLocalShare = dataDec->getBool();    //技能CD

            px = mx / 2;
            py = my / 2;

            socTools->resetUDPIP();
            socTools->addUDPIP("127.0.0.1:8251");

            if (isShare) {
                gp = dataDec->getLong();
                sprintf(ss, "%s:%d", is, 8251);
                socTools->setGroup(gp);
                socTools->addUDPIP(ss);
            } else if (isLocalShare) {
                localIP = dataDec->getStr();
                sprintf(ss, "%s:%d", localIP, 8251);
                delete[] localIP;
                socTools->addUDPIP(ss);
            }
            callFun();

        } else if (cmd == STOP) {
            if (webIsRun) {
                websocket_server_stop(&sserver);
                http_server_stop(&wserver);
            }
            localIsRun = false;
            webIsRun = false;
        } else if (cmd == BUFF_DISPLY) {
            buffDisply = dataDec->getInt();
        } else if (cmd == CD_DISPLAY) {
            cdDisply = dataDec->getInt();    //技能CD
        } else if (cmd == WEB_SERVICE) {
            isWeb = dataDec->getInt();
            if (isWeb == 1) {
                localIsRun = false;
            } else {
                webIsRun = false;
                websocket_server_stop(&sserver);
                http_server_stop(&wserver);
            }
            callFun();
        }
    }

    //开始读取数据并发送
    void getData() {
        DataEnc *dataEnc = socTools->getDataEnc();
        if (!iGame->isRunn()) return;
        iGame->init(true);
        // 发送阵营
        socTools->sendCMD(iGame->getCamp() == 1 ? D_BLUE : D_RED);
        int count = iGame->getHeroCount();

        if (count > 11) {
#if P == 1
            sprintf(buff, "游戏数据校验失败 玩家数量:%d", count);
            addLog(buff);
#endif
            //游戏数据校验失败，可能游戏更新
            socTools->sendCMD(D_MESSGE, "游戏数据校验失败，游戏可能更新");
            closeAll();
        }
        // buff CD
        Buffm buffm{};
        int rrType[4] = {3, 3, 3, 3};
        int rrFTime[4] = {0, 0, 0, 0};
        int rrFTimeT[4] = {0, 0, 0, 0};
        int buffTimes[4] = {0, 0, 0, 0};

        mlong curTime = TimeTools::getCurrentTime();
        mlong preTime = 0;

        while (localIsRun) {
            if (!iGame->isRunn()) {
#if P == 1
                addLog("游戏结束");
                addLog("============================================");
                pushLog();
#endif
                return;
            }

            // 读取Buff
            if (buffDisply) {
                // 获取buff时间
                iGame->getBuff(buffm);
                // 四个buff时间
                buffTimes[0] = buffm.bbTime;
                buffTimes[1] = buffm.brTime;
                buffTimes[2] = buffm.rbTime;
                buffTimes[3] = buffm.rrTime;

                preTime = curTime;
                curTime = TimeTools::getCurrentTime();

                for (int i = 0; i < 4; i++) {
                    if (buffTimes[i] == 95000 && rrType[i] != 3) {
                        // 表示buff倒计时结束
                        if (rrType[i] == 1) {
                            buffTimes[i] = rrFTime[i] = 3;
                            rrFTimeT[i] = 0;
                            rrType[i] = 2;
                        } else if (rrType[i] == 2) {
                            rrFTimeT[i] += (int) (curTime - preTime);
                            if (rrFTimeT[i] >= 1000) {
                                rrFTime[i]--;
                                rrFTimeT[i] = 0;
                            }
                            buffTimes[i] = rrFTime[i];
                            if (rrFTime[i] == 0) {
                                rrType[i] = 3;
                            }
                        }
                    } else {
                        buffTimes[i] = (buffTimes[i] == 95000 ? 0 : ((buffTimes[i] / 1000) + 4));
                        if (buffTimes[i] > 0) {
                            rrType[i] = 1;
                        }
                    }
                }
            } else {
                // 重置buff时间
                memset(buffTimes, 0, sizeof(int) * 4);
            }
            // 临时变量
            Hero hero{};
            for (int i = 0; i < count; i++) {
                iGame->getHeroData(hero, i, true);
                // 打包数据
                printf("id:%d x:%d y:%d hp:%d\n", hero.id, hero.x, hero.y, hero.hp);
                dataEnc->putInt(hero.id)
                        .putInt(hero.x)
                        .putInt(hero.y)
                        .putInt(hero.hp)
                        .putInt((int) hero.c2d.x)
                        .putInt((int) hero.c2d.y)
                        .putInt((int) hero.c2d.h)
                        .putInt(hero.dts)
                        .putInt(hero.dt)
                        .putInt(buffTimes[0])
                        .putInt(buffTimes[1])
                        .putInt(buffTimes[2])
                        .putInt(buffTimes[3]);
                // 数据数量加一
                socTools->addCrs();
            }
            printf("============\n");
            // 发送数据
            socTools->sendData(D_DATA);
            isClear = false;
            if (isShare) {
                // 延时
                TimeTools::sleep_ms(50);
            } else {
                // 延时
                TimeTools::sleep_ms(40);
            }
        }
    }
};


class MyContext {
public:
    //buff CD
    Buffm buffm{};
    int rrType[4] = {3, 3, 3, 3};
    int rrFTime[4] = {0, 0, 0, 0};
    int rrFTimeT[4] = {0, 0, 0, 0};
    int buffTimes[4] = {0, 0, 0, 0};

    mlong curTime = TimeTools::getCurrentTime();
    mlong preTime = 0;
    bool newGameDataIsload = false;
    bool isReturn = false;

    TimerID timerID;

    MyContext() {
        timerID = INVALID_TIMER_ID;
    }

    void init(const WebSocketChannelPtr &channel) {
        if (!baseIsInit) {
            iGame = new IGame(nullptr);
            bool initBase = iGame->initBase();
            //进程不存在
            if (!initBase) {
                //printf("进程不存在\n");
                Json data;
                data["cmd"] = -2;
                data["messge"] = "游戏未运行";

                std::ostringstream stream;
                stream << data;
                string js = stream.str();
                channel->send(js.c_str());

                websocket_server_stop(&sserver);
                http_server_stop(&wserver);
                return;
            }
            baseIsInit = true;
        }
    }


    int flag = 0;

    void run(const WebSocketChannelPtr &channel) {
        if (isReturn) {
            if (flag < 60) {
                flag++;
                return;
            }
        }

        flag = 0;
        if (!iGame->isRunn()) {
            newGameDataIsload = false;
            isReturn = true;
            Json data;
            data["cmd"] = -1;

            std::ostringstream stream;
            stream << data;
            string js = stream.str();
            channel->send(js.c_str());
            pushLog();
            return;
        }

        if (!newGameDataIsload) {
            iGame->init(false);
            newGameDataIsload = true;
        }

        isReturn = false;

        // 读取Buff
        // 读取Buff
        if (buffDisply) {

            iGame->getBuff(buffm);

            buffTimes[0] = buffm.bbTime;
            buffTimes[1] = buffm.brTime;
            buffTimes[2] = buffm.rbTime;
            buffTimes[3] = buffm.rrTime;

            preTime = curTime;
            curTime = TimeTools::getCurrentTime();

            for (int i = 0; i < 4; i++) {
                if (buffTimes[i] == 95000 && rrType[i] != 3) {
                    //表示buff倒计时结束
                    if (rrType[i] == 1) {
                        buffTimes[i] = rrFTime[i] = 3;
                        rrFTimeT[i] = 0;
                        rrType[i] = 2;
                    } else if (rrType[i] == 2) {
                        rrFTimeT[i] += (int) (curTime - preTime);
                        if (rrFTimeT[i] >= 1000) {
                            rrFTime[i]--;
                            rrFTimeT[i] = 0;
                        }
                        buffTimes[i] = rrFTime[i];
                        if (rrFTime[i] == 0) {
                            rrType[i] = 3;
                        }
                    }
                } else {
                    buffTimes[i] = (buffTimes[i] == 95000 ? 0 : ((buffTimes[i] / 1000) + 4));
                    if (buffTimes[i] > 0) {
                        rrType[i] = 1;
                    }
                }
            }

        } else {
            memset(buffTimes, 0, sizeof(int) * 4);
        }


        Json data;

        Hero hero{};
        for (int i = 0; i < iGame->getHeroCount(); i++) {
            iGame->getHeroData(hero, i);

            Json heroJson;
            if (iGame->getCamp() == 2) {
                if (hero.camp == 1) {
                    heroJson["cmap"] = 2;
                } else {
                    heroJson["cmap"] = 1;
                }
            } else {
                heroJson["cmap"] = hero.camp;
            }

            heroJson["id"] = hero.id;
            heroJson["x"] = hero.x;
            heroJson["y"] = hero.y;
            heroJson["hp"] = hero.hp;

            data["listData"] += {heroJson};
            //printf("id:%d x:%d y:%d hp:%d\n", hero.id, hero.x, hero.y, hero.hp);
        }

        Json gameBuff;
        gameBuff["bbTime"] = buffTimes[0];
        gameBuff["brTime"] = buffTimes[1];
        gameBuff["rbTime"] = buffTimes[2];
        gameBuff["rrTime"] = buffTimes[3];
        data["buff"] = gameBuff;
        data["cmd"] = 1;

        std::ostringstream stream;
        stream << data;
        string js = stream.str();
        channel->send(js.c_str());

    }

    int handleMessage(const std::string &msg) {
        const char *vfloat = "vfloat";
        const char *bfloat = "bfloat";
        if (socTools != nullptr) {
            if (strstr(msg.c_str(), vfloat) != nullptr) {
                socTools->sendCMD(D_FLAT_VIS, 1);
            } else if (strstr(msg.c_str(), bfloat) != nullptr) {
                socTools->sendCMD(D_FLAT_VIS, 0);
            }
        }
        return msg.size();
    }

};


void *webServer(void *) {

    HttpService router;
    router.GET("/image/*.png", [](HttpRequest *req, HttpResponse *resp) {
        string path = req->Path().substr(1, req->Path().size());
        return resp->File(path.c_str());
    });

    router.GET("/", [&router](HttpRequest *req, HttpResponse *resp) {
        string path = "web/index.html";
        HFile file;
        if (file.open(path.c_str(), "rb") != 0) {
            return 500;
        }
        string body;
        char ip[50];
        getStbIP(ip);
        file.readall(body);
        replace_all(body, "127.0.0.1", ip);
        resp->body = body;
        const char *suffix = hv_suffixname(path.c_str());
        if (suffix) {
            resp->content_type = http_content_type_enum_by_suffix(suffix);
        }
        if (resp->content_type == CONTENT_TYPE_NONE ||
            resp->content_type == CONTENT_TYPE_UNDEFINED) {
            resp->content_type = APPLICATION_OCTET_STREAM;
        }
        return 200;
    });

    wserver.service = &router;
    wserver.port = 8080;

    http_server_run(&wserver);
    return nullptr;
}

int deivcCount = 0;

void *websocketServer(void *v) {
    hmutex_lock(&listlocker);

    pthread_t web;
    pthread_create(&web, nullptr, webServer, nullptr);

    webIsRun = true;
    WebSocketServerCallbacks ws;
    ws.onopen = [](const WebSocketChannelPtr &channel, const std::string &url) {
        auto *ctx = channel->newContext<MyContext>();
        if (deivcCount < 1) {
            ctx->init(channel);
            ctx->timerID = setInterval(50, [channel](TimerID id) {
                auto *context = channel->getContext<MyContext>();
                context->run(channel);
            });
        }
        deivcCount++;
    };
    ws.onmessage = [](const WebSocketChannelPtr &channel, const std::string &msg) {
        auto *ctx = channel->getContext<MyContext>();
        ctx->handleMessage(msg);
    };

    ws.onclose = [](const WebSocketChannelPtr &channel) {
        auto *ctx = channel->getContext<MyContext>();
        if (ctx->timerID != INVALID_TIMER_ID) {
            killTimer(ctx->timerID);
        }
        channel->deleteContext<MyContext>();
        deivcCount--;
    };

    sserver.port = 9091;
    sserver.ws = &ws;
    websocket_server_run(&sserver);
    hmutex_unlock(&listlocker);
    return nullptr;
}


int main(int argc, char *argv[]) {
    printf("abi:%d\n", abiUtil::abi(32, 64));
    Engine engine;
}
