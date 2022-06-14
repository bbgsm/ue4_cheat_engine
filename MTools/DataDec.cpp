#include "DataDec.h"
#include "ByteUtil.h"

#include <unistd.h>
#include <cstring>

DataDec::DataDec() {
}


DataDec::DataDec(mbyte *bytes, int bytelen) {
    setData(bytes, bytelen);
}


void DataDec::setData(mbyte *bytes, int bytelen) {
    reset();
    m_byteLen = bytelen;
    m_bytes = bytes;
}

int DataDec::getInt() {
    int val = getInt(index);
    index += 4;
    return val;
}

mlong DataDec::getLong() {
    mlong val = getLong(index);
    index += 8;
    return val;
}

mbyte DataDec::getByte() {
    mbyte val = getByte(index);
    index += 1;
    return val;
}

bool DataDec::getBool() {
    return getByte() > 0;
}

char *DataDec::getStr() {
    int len = getInt();
    if (len > 0 && (index + len) <= m_byteLen) {
        char *str = new char[len + 1];
        memset(str, 0, len);
        memcpy(str, m_bytes + index, len);
        str[len] = '\0';
        index += len;
        return str;
    }
    return nullptr;
}

float DataDec::getFloat() {
    float val = getFloat(index);
    index += 4;
    return val;
}

double DataDec::getDouble() {
    double val = getDouble(index);
    index += 8;
    return val;
}


int DataDec::getCmd() {
    return getInt(0);
}

int DataDec::getCount() {
    return getInt(4);
}

mlong DataDec::getLength() {
    return getLong(8);
}

void DataDec::skip(int off) {
    index = HEADER_LEN + off;
}

void DataDec::reset() {
    index = HEADER_LEN;
}

int DataDec::headerSize() {
    return HEADER_LEN;
}

int DataDec::getInt(int i) {
    if ((i + 4) <= m_byteLen) {
        int val = ByteUtil::bytesToInt(m_bytes, i);
        return val;
    }
    return 0;
}

mlong DataDec::getLong(int i) {
    if ((i + 8) <= m_byteLen) {
        mlong lg = ByteUtil::bytesToLong(m_bytes, i);
        return lg;
    }
    return 0;
}

mbyte DataDec::getByte(int i) {
    if ((i + 1) <= m_byteLen) {
        mbyte b = m_bytes[i];
        return b;
    }
    return 0;
}

char *DataDec::getStr(int i) {
    int len = getInt(i);
    i += 4;
    if (len > 0 && (i + len) <= m_byteLen) {
        char *str = new char[len];
        memcpy(str, m_bytes + i, len);
        str[len] = '\0';
        return str;
    }
    return nullptr;
}

float DataDec::getFloat(int i) {
    return getInt(i) / 1000;
}

double DataDec::getDouble(int i) {
    return getLong(i) / 1000000;
}


