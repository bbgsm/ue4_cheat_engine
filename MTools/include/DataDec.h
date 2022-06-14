
#include "Type.h"

#ifndef DATADEC_H
#define DATADEC_H


class DataDec {
private:
    static const int HEADER_LEN = 16;
    mbyte *m_bytes;
    int index = HEADER_LEN;
    int m_byteLen = 0;

public:
    int getCmd();
    int getCount();
    mlong getLength();

    DataDec();
    DataDec(mbyte *bytes,int bytelen);
    void setData(mbyte *bytes,int bytelens);

    int getInt();                      //获取一个int
    mlong getLong();                   //获取一个long
    mbyte getByte();                   //获取一个byte
    bool getBool();                   //获取一个byte
    char *getStr();                    //获取字符 字符内存空间为new 需要自行清理
    float getFloat();                  //获取一个float
    double getDouble();                //获取一个double

    int getInt(int i);                      //获取下标的int
    mlong getLong(int i);                   //获取下标的long
    mbyte getByte(int i);                   //获取下标的byte
    char *getStr(int i);                    //获取下标的字符 字符内存空间为new 需要自行清理
    float getFloat(int i);                  //获取下标的float
    double getDouble(int i);                //获取下标的double

    static int headerSize();            //头大小
    void reset();                       //重置读取下标
    void skip(int off);                 //偏移读取下标
};


#endif
