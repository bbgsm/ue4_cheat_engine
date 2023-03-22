#pragma once

typedef signed char mbyte;
typedef long long mlong;       //long 8
typedef unsigned long ulong;  //unsig long 4/8
typedef ulong Addr;  //unsig long 4/8
typedef long Offset;  //unsig long 4/8
typedef double mdouble;       //double 8
typedef float mfloat;         //float 4
typedef unsigned int uint;
typedef int TYPE;
typedef unsigned char boolean;
typedef unsigned char uchar;
typedef unsigned int CharType;
typedef unsigned char UTF8;
typedef unsigned short UTF16;
typedef unsigned int UTF32;
typedef signed short int int16;

// 3D结构体
struct C3D {
    float x;
    float y;
    float z;
};

// 2D结构体
struct C2D {
    float x;
    float y;
    float w;
    float h;
};

struct RADDR {
    Addr addr;                    // 起始地址
    Addr taddr;                   // 结束地址
};



