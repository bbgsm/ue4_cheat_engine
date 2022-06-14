//
// Created by NotAdmin on 2021/10/6.
//

#include "BinCCation.h"

bool BinCCation::sendMessge(char *str, int toastType) {
    return sendMessge(str, strlen(str), toastType);;
}

bool BinCCation::sendMessge(char *str, int len, int toastType) {
    DataEnc *dataEnc = getDataEnc();
    dataEnc->reset();
    dataEnc->setCmd(D_MESSGE);
    dataEnc->setCount(0);
    dataEnc->putStr(str, len);
    dataEnc->putInt(toastType);
    return sendData();
}
