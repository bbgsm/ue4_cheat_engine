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


// CMD??????
enum CMD {
    START,                       // ?????? ????????????
    STOP,                        // ??????
    ZM,                          // ??????
    ZM_RANGR,                    // ????????????
    ZM_H,                        // ????????????
    ZM_V,                        // ????????????
    ZM_SPEED,                    // ??????????????????
    ZM_MODE,                     // ????????????
    FPS,                         // FPS
    IS_BONE,                     // ????????????
    IS_VEHICLE,                  // ?????????
    IS_MATERIAL,                 // ????????????
    ZM_SELECT_TYPE,              // ????????????
    ZM_TOUCH_POSITTON,           // ??????????????????
    IS_ZM_DOWM,                  // ??????????????????
    IS_ZM_COHERENT               // ?????????????????????????????????????????????
};

enum DATA_INTERACTION {
    D_DATA,                        // ????????????
    D_EXIT,                        // ????????????
    D_PROISNOT,                    // ???????????????
    D_CLEAR,                       // ????????????
    D_MESSGE
};

struct ObjectData {
    int type = 0;          //????????????
    int scid = 0;          //????????????
    C2D position{0, 0, 0, 0}; //??????????????????
    float headerX = 0;     //????????????????????????
    float headerY = 0;
    float d3X;             // ??????X
    float d3Y;             // ??????Y
    int distance = 0;      //????????????
    float health = 0;      //????????????
    int isbot = 0;         //???????????????
    int isSelect = 0;      //??????????????????
    int teamId = 0;        //??????ID
    int nicklen = 0;       //????????????
    char nickName[50] = "";//??????
    Bone2D bone2D;         //??????
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
    //????????????
    dataEnc->putInt(objectData.type);
    //??????id
    dataEnc->putInt(objectData.scid);
    //????????????
    dataEnc->putFloat(objectData.position.x);
    dataEnc->putFloat(objectData.position.y);

    //??????????????????????????????
    dataEnc->putInt(objectData.headerX);
    dataEnc->putInt(objectData.headerY);

    //??????????????????????????????
    dataEnc->putFloat(objectData.d3X);
    dataEnc->putFloat(objectData.d3Y);

    //????????????
    dataEnc->putFloat(objectData.position.w);
    dataEnc->putFloat(objectData.position.h);
    //??????
    dataEnc->putInt(objectData.distance);
    //??????
    dataEnc->putInt((int) objectData.health);
    //????????????
    dataEnc->putInt(!objectData.isbot);
    //????????????
    dataEnc->putInt(objectData.isSelect);
    //??????id
    dataEnc->putInt(objectData.teamId);

    if (isBone) {
        //??????
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
        //??????
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
    dir = opendir("/proc");        // ????????????
    if (NULL != dir) {
        while ((ptr = readdir(dir)) != NULL)    // ???????????????????????????????????????/?????????
        {
            // ?????????????????????"."??????".."??????????????????????????????????????????????????????
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                continue;
            if (ptr->d_type != DT_DIR)
                continue;
            sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);    // ?????????????????????????????????
            fp = fopen(filepath, "r");    // ????????????
            if (NULL != fp) {
                fgets(filetext, sizeof(filetext), fp);    // ????????????
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

    closedir(dir);                // ????????????
}


// ???UTF16 LE????????????????????????
string utf16le_to_utf8(const u16string &u16str) {
    if (u16str.empty()) { return string(); }
    const char16_t *p = u16str.data();
    u16string::size_type len = u16str.length();
    if (p[0] == 0xFEFF) {
        p += 1;    //??????bom???????????????
        len -= 1;
    }

    // ????????????
    string u8str;
    u8str.reserve(len * 3);

    char16_t u16char;
    for (u16string::size_type i = 0; i < len; ++i) {
        // ??????????????????????????????(??????????????????)
        u16char = p[i];

        // 1??????????????????
        if (u16char < 0x0080) {
            // u16char <= 0x007f
            // U- 0000 0000 ~ 0000 07ff : 0xxx xxxx
            u8str.push_back((char) (u16char & 0x00FF));    // ??????8bit
            continue;
        }
        // 2 ?????????????????????
        if (u16char >= 0x0080 && u16char <= 0x07FF) {
            // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
            u8str.push_back((char) (((u16char >> 6) & 0x1F) | 0xC0));
            u8str.push_back((char) ((u16char & 0x3F) | 0x80));
            continue;
        }
        // ??????????????????(4????????????)
        if (u16char >= 0xD800 && u16char <= 0xDBFF) {
            // * U-00010000 - U-001FFFFF: 1111 0xxx 10xxxxxx 10xxxxxx 10xxxxxx
            uint32_t highSur = u16char;
            uint32_t lowSur = p[++i];
            // ??????????????????UNICODE???????????????
            // 1????????????????????????0xD800???????????????10bit
            // 2????????????????????????0xDC00???????????????10bit
            // 3?????????0x10000?????????UNICODE????????????
            uint32_t codePoint = highSur - 0xD800;
            codePoint <<= 10;
            codePoint |= lowSur - 0xDC00;
            codePoint += 0x10000;
            // ??????4??????UTF8????????????
            u8str.push_back((char) ((codePoint >> 18) | 0xF0));
            u8str.push_back((char) (((codePoint >> 12) & 0x3F) | 0x80));
            u8str.push_back((char) (((codePoint >> 06) & 0x3F) | 0x80));
            u8str.push_back((char) ((codePoint & 0x3F) | 0x80));
            continue;
        }
        // 3 ??????????????????
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



