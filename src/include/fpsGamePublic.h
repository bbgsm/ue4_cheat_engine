#include "DataEnc.h"
#include <GameTools.h>
#include "MemoryTools.h"
#include "TimeTools.h"
#include <dirent.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>


#ifndef GAME_PUBLIC_H
#define GAME_PUBLIC_H

#define FALSE  0
#define TRUE   1

#define halfShift    10
#define UNI_SUR_HIGH_START  (UTF32)0xD800
#define UNI_SUR_HIGH_END    (UTF32)0xDBFF
#define UNI_SUR_LOW_START   (UTF32)0xDC00
#define UNI_SUR_LOW_END     (UTF32)0xDFFF
/* Some fundamental constants */
#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD
#define UNI_MAX_BMP (UTF32)0x0000FFFF
#define UNI_MAX_UTF16 (UTF32)0x0010FFFF
#define UNI_MAX_UTF32 (UTF32)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (UTF32)0x0010FFFF

typedef unsigned char boolean;
typedef unsigned int CharType;
typedef unsigned char UTF8;
typedef unsigned short UTF16;
typedef unsigned int UTF32;

static const UTF32 halfMask = 0x3FFUL;
static const UTF32 halfBase = 0x0010000UL;
static const UTF8 firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
static const UTF32 offsetsFromUTF8[6] = {0x00000000UL, 0x00003080UL, 0x000E2080UL, 0x03C82080UL, 0xFA082080UL,
                                         0x82082080UL};
static const char trailingBytesForUTF8[256] =
        {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
        };
typedef enum {
    strictConversion = 0,
    lenientConversion
} ConversionFlags;
typedef enum {
    conversionOK,         /* conversion successful */
    sourceExhausted,      /* partial character in source, but hit end */
    targetExhausted,      /* insuff. room in target for conversion */
    sourceIllegal,        /* source sequence is illegal/malformed */
    conversionFailed
} ConversionResult;


// CMD命令
enum CMD {
    START,                       // 开始 搜索内存
    STOP,                        // 结束
    ZM,                          // 自瞄
    ZM_RANGR,                    // 自瞄范围
    ZM_H,                        // 自瞄横向
    ZM_V,                        // 自瞄纵向
    ZM_SPEED,                    // 自瞄滑动速度
    ZM_MODE,                     // 自瞄模式
    FPS,                         // FPS
    IS_BONE,                     // 显示骨骼
    IS_VEHICLE,                  // 显示车
    IS_MATERIAL,                 // 显示物资
    ZM_SELECT_TYPE,              // 自瞄选中
    ZM_TOUCH_POSITTON,           // 触摸开始坐标
    IS_ZM_DOWM,                  // 是否瞄准倒地
    IS_ZM_COHERENT               // 击败玩家是否继续瞄准下一个玩家
};

enum DATA_INTERACTION {
    D_DATA,                        // 游戏数据
    D_EXIT,                        // 游戏结束
    D_PROISNOT,                    // 进程不存在
    D_CLEAR,                       // 清理视图
    D_MESSGE
};

struct ObjectData {
    int type = 0;          //对象类型
    int scid = 0;          //手持判断
    C2D position{0, 0, 0, 0}; //对象屏幕坐标
    float headerX = 0;     //对象头的屏幕坐标
    float headerY = 0;
    float d3X;             // 三维X
    float d3Y;             // 三维Y
    int distance = 0;      //对象距离
    float health = 0;      //对象血量
    int isbot = 0;         //是否是人机
    int isSelect = 0;      //准星是否选中
    int teamId = 0;        //团队ID
    int nicklen = 0;       //昵称长度
    char nickName[50] = "";//昵称
    Bone2D bone2D;         //骨骼
};

void resrtBone(Bone2D &bone2D) {
    memset(&bone2D, 0, sizeof(Bone2D));

/*    bone2D.Head = {0, 0, 0, 0};
    bone2D.Pit = {0, 0, 0, 0};
    bone2D.Pelvis = {0, 0, 0, 0};
    bone2D.Lcollar = {0, 0, 0, 0};
    bone2D.Rcollar = {0, 0, 0, 0};
    bone2D.Lelbow = {0, 0, 0, 0};
    bone2D.Relbow = {0, 0, 0, 0};
    bone2D.Lwrist = {0, 0, 0, 0};
    bone2D.Rwrist = {0, 0, 0, 0};
    bone2D.Lthigh = {0, 0, 0, 0};
    bone2D.Rthigh = {0, 0, 0, 0};
    bone2D.Lknee = {0, 0, 0, 0};
    bone2D.Rknee = {0, 0, 0, 0};
    bone2D.Lankle = {0, 0, 0, 0};
    bone2D.Rankle = {0, 0, 0, 0};*/
}

void reset(ObjectData &objectData) {
    objectData.type = 0;
    objectData.scid = 0;
    objectData.position = {0, 0, 0, 0};
    objectData.headerX = 0;
    objectData.headerY = 0;
    objectData.d3X = 0;
    objectData.d3Y = 0;
    objectData.distance = 0;
    objectData.health = 0;
    objectData.isbot = 0;
    objectData.isSelect = 0;
    objectData.teamId = 0;
    objectData.nicklen = 0;
    memset(objectData.nickName, 0, 50);
}


void putSortData(DataEnc *dataEnc, ObjectData &objectData, int isBone) {
    //对象类型
    dataEnc->putInt(objectData.type);
    //手持id
    dataEnc->putInt(objectData.scid);
    //方框坐标
    dataEnc->putFloat(objectData.position.x);
    dataEnc->putFloat(objectData.position.y);

    //头部坐标（暂时不用）
    dataEnc->putInt(objectData.headerX);
    dataEnc->putInt(objectData.headerY);

    //三维坐标（暂时不用）
    dataEnc->putFloat(objectData.d3X);
    dataEnc->putFloat(objectData.d3Y);

    //方框宽高
    dataEnc->putFloat(objectData.position.w);
    dataEnc->putFloat(objectData.position.h);
    //距离
    dataEnc->putInt(objectData.distance);
    //血量
    dataEnc->putInt((int) objectData.health);
    //人机判断
    dataEnc->putInt(!objectData.isbot);
    //是否选中
    dataEnc->putInt(objectData.isSelect);
    //团队id
    dataEnc->putInt(objectData.teamId);

    if (isBone) {
        //骨骼
        dataEnc->putInt((int) objectData.bone2D.Head.x);
        dataEnc->putInt((int) objectData.bone2D.Head.y);

        dataEnc->putInt((int) objectData.bone2D.Pit.x);
        dataEnc->putInt((int) objectData.bone2D.Pit.y);

        dataEnc->putInt((int) objectData.bone2D.Pelvis.x);
        dataEnc->putInt((int) objectData.bone2D.Pelvis.y);

        dataEnc->putInt((int) objectData.bone2D.Lcollar.x);
        dataEnc->putInt((int) objectData.bone2D.Lcollar.y);

        dataEnc->putInt((int) objectData.bone2D.Rcollar.x);
        dataEnc->putInt((int) objectData.bone2D.Rcollar.y);

        dataEnc->putInt((int) objectData.bone2D.Lelbow.x);
        dataEnc->putInt((int) objectData.bone2D.Lelbow.y);

        dataEnc->putInt((int) objectData.bone2D.Relbow.x);
        dataEnc->putInt((int) objectData.bone2D.Relbow.y);

        dataEnc->putInt((int) objectData.bone2D.Lwrist.x);
        dataEnc->putInt((int) objectData.bone2D.Lwrist.y);

        dataEnc->putInt((int) objectData.bone2D.Rwrist.x);
        dataEnc->putInt((int) objectData.bone2D.Rwrist.y);

        dataEnc->putInt((int) objectData.bone2D.Lthigh.x);
        dataEnc->putInt((int) objectData.bone2D.Lthigh.y);

        dataEnc->putInt((int) objectData.bone2D.Rthigh.x);
        dataEnc->putInt((int) objectData.bone2D.Rthigh.y);

        dataEnc->putInt((int) objectData.bone2D.Lknee.x);
        dataEnc->putInt((int) objectData.bone2D.Lknee.y);

        dataEnc->putInt((int) objectData.bone2D.Rknee.x);
        dataEnc->putInt((int) objectData.bone2D.Rknee.y);

        dataEnc->putInt((int) objectData.bone2D.Lankle.x);
        dataEnc->putInt((int) objectData.bone2D.Lankle.y);

        dataEnc->putInt((int) objectData.bone2D.Rankle.x);
        dataEnc->putInt((int) objectData.bone2D.Rankle.y);
    } else {
        //骨骼
        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

        dataEnc->putInt(0);
        dataEnc->putInt(0);

    }


}

bool isSelect(int dpx, int dpy, C2D obj) {
    if (((dpx >= obj.x - obj.w)
         && (dpx <= obj.x + obj.w))
        && ((dpy >= obj.y - obj.h)
            && (dpy <= obj.y + obj.h))) {
        return true;
    }
    return false;
}

void fpsDelay(mlong start, mlong end, int fps) {
    if ((1000 / fps) > (end - start)) {
        TimeTools::sleep_ms((1000 / fps) - (end - start) + 2);
    } else {
        TimeTools::sleep_ms(2);
    }
}

void killInvalidSelf(const char *bm) {
    DIR *dir = NULL;
    dirent *ptr = NULL;
    FILE *fp = NULL;
    char filepath[256];
    char filetext[128];
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


// 从UTF16 LE编码的字符串创建
string utf16le_to_utf8(const u16string &u16str) {
    if (u16str.empty()) { return string(); }
    const char16_t *p = u16str.data();
    u16string::size_type len = u16str.length();
    if (p[0] == 0xFEFF) {
        p += 1;    //带有bom标记，后移
        len -= 1;
    }

    // 开始转换
    string u8str;
    u8str.reserve(len * 3);

    char16_t u16char;
    for (u16string::size_type i = 0; i < len; ++i) {
        // 这里假设是在小端序下(大端序不适用)
        u16char = p[i];

        // 1字节表示部分
        if (u16char < 0x0080) {
            // u16char <= 0x007f
            // U- 0000 0000 ~ 0000 07ff : 0xxx xxxx
            u8str.push_back((char) (u16char & 0x00FF));    // 取低8bit
            continue;
        }
        // 2 字节能表示部分
        if (u16char >= 0x0080 && u16char <= 0x07FF) {
            // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
            u8str.push_back((char) (((u16char >> 6) & 0x1F) | 0xC0));
            u8str.push_back((char) ((u16char & 0x3F) | 0x80));
            continue;
        }
        // 代理项对部分(4字节表示)
        if (u16char >= 0xD800 && u16char <= 0xDBFF) {
            // * U-00010000 - U-001FFFFF: 1111 0xxx 10xxxxxx 10xxxxxx 10xxxxxx
            uint32_t highSur = u16char;
            uint32_t lowSur = p[++i];
            // 从代理项对到UNICODE代码点转换
            // 1、从高代理项减去0xD800，获取有效10bit
            // 2、从低代理项减去0xDC00，获取有效10bit
            // 3、加上0x10000，获取UNICODE代码点值
            uint32_t codePoint = highSur - 0xD800;
            codePoint <<= 10;
            codePoint |= lowSur - 0xDC00;
            codePoint += 0x10000;
            // 转为4字节UTF8编码表示
            u8str.push_back((char) ((codePoint >> 18) | 0xF0));
            u8str.push_back((char) (((codePoint >> 12) & 0x3F) | 0x80));
            u8str.push_back((char) (((codePoint >> 06) & 0x3F) | 0x80));
            u8str.push_back((char) ((codePoint & 0x3F) | 0x80));
            continue;
        }
        // 3 字节表示部分
        {
            // * U-0000E000 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
            u8str.push_back((char) (((u16char >> 12) & 0x0F) | 0xE0));
            u8str.push_back((char) (((u16char >> 6) & 0x3F) | 0x80));
            u8str.push_back((char) ((u16char & 0x3F) | 0x80));
            continue;
        }
    }

    return u8str;
}

int Utf16_To_Utf8(const UTF16 *sourceStart, UTF8 *targetStart, size_t outLen, ConversionFlags flags) {
    int result = 0;
    const UTF16 *source = sourceStart;
    UTF8 *target = targetStart;
    UTF8 *targetEnd = targetStart + outLen;

    if ((nullptr == source) || (nullptr == targetStart)) {
        printf("ERR, Utf16_To_Utf8: source=%p, targetStart=%p\n", source, targetStart);
        return conversionFailed;
    }

    while (*source) {
        UTF32 ch;
        unsigned short bytesToWrite = 0;
        const UTF32 byteMask = 0xBF;
        const UTF32 byteMark = 0x80;
        const UTF16 *oldSource = source; /* In case we have to back up because of target overflow. */
        ch = *source++;
        /* If we have a surrogate pair, convert to UTF32 first. */
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            /* If the 16 bits following the high surrogate are in the source buffer... */
            if (*source) {
                UTF32 ch2 = *source;
                /* If it's a low surrogate, convert to UTF32. */
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
                    ++source;
                } else if (flags == strictConversion) { /* it's an unpaired high surrogate */
                    --source; /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                }
            } else { /* We don't have the 16 bits following the high surrogate. */
                --source; /* return to the high surrogate */
                result = sourceExhausted;
                break;
            }
        } else if (flags == strictConversion) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                --source; /* return to the illegal value itself */
                result = sourceIllegal;
                break;
            }
        }
        /* Figure out how many bytes the result will require */
        if (ch < (UTF32) 0x80) {
            bytesToWrite = 1;
        } else if (ch < (UTF32) 0x800) {
            bytesToWrite = 2;
        } else if (ch < (UTF32) 0x10000) {
            bytesToWrite = 3;
        } else if (ch < (UTF32) 0x110000) {
            bytesToWrite = 4;
        } else {
            bytesToWrite = 3;
            ch = UNI_REPLACEMENT_CHAR;
        }

        target += bytesToWrite;
        if (target > targetEnd) {
            source = oldSource; /* Back up source pointer! */
            target -= bytesToWrite;
            result = targetExhausted;
            break;
        }
        switch (bytesToWrite) { /* note: everything falls through. */
            case 4:
                *--target = (UTF8) ((ch | byteMark) & byteMask);
                ch >>= 6;
            case 3:
                *--target = (UTF8) ((ch | byteMark) & byteMask);
                ch >>= 6;
            case 2:
                *--target = (UTF8) ((ch | byteMark) & byteMask);
                ch >>= 6;
            case 1:
                *--target = (UTF8) (ch | firstByteMark[bytesToWrite]);
        }
        target += bytesToWrite;
    }
    return result;
}

#endif



