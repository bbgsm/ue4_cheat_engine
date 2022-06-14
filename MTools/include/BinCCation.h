#ifndef EP_BINCCATION_H
#define EP_BINCCATION_H

#include "SocketTools.h"

enum DATA_INTERACTION {
    D_DATA = 0,                        // 游戏数据
    D_EXIT = 1,                        // 游戏结束
    D_PROISNOT = 2,                    // 进程不存在
    D_CLEAR = 3,                       // 清理视图
    D_MESSGE = 4
};

enum TOAST_TYPE {
    SUCCESS = 0,
    ERROR = 1,
    WARNING = 2,
    INFO = 3
};

class BinCCation : public SocketTools {
public:
    bool sendMessge(char *str, int toastType = SUCCESS);

    bool sendMessge(char *str, int len, int toastType = SUCCESS);
};


#endif
