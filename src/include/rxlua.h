#include "MemoryTools.h"
#include "SocketTools.h"
#include "TimeTools.h"


extern "C" {

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

lua_State *g_L;

MemoryTools *memoryTools;
SocketTools *g_socTools;
DataEnc *dataEnc;


int getDataEnc(lua_State *L) {
    Addr addr = (Addr) dataEnc;
    lua_pushnumber(L, addr);
    return 1;
}

int putI(lua_State *L) {
    float val = luaL_checknumber(L, 1);
    dataEnc->putInt((int) val);
    return 0;
}

int addCrs(lua_State *L) {
    g_socTools->addCrs();
    return 0;
}

int sendData(lua_State *L) {
    int cmd = luaL_checknumber(L, 1);
    g_socTools->sendData(cmd);
    return 0;
}

int sendCMD(lua_State *L) {
    int cmd = luaL_checknumber(L, 1);
    g_socTools->sendCMD(cmd);
    return 0;
}

bool m_get3Dto2D(C2D &c2d, const float *Matrix, const float px, const float py, const C3D &obj) {
    float Raito1 = obj.x * Matrix[3] + obj.y * Matrix[7] + obj.z * Matrix[11] + Matrix[15];
    if (Raito1 < 0.01) {
        c2d = {9999, 9999, 0, 0};
        return false;
    }
    c2d.x = px + (obj.x * Matrix[0] + obj.y * Matrix[4] + obj.z * Matrix[8] + Matrix[12]) / Raito1 * px;
    c2d.y = py - (obj.x * Matrix[1] + (obj.y) * Matrix[5] + obj.z * Matrix[9] + Matrix[13]) / Raito1 * py;
    c2d.w = py - (obj.x * Matrix[1] + (obj.y + 4.0) * Matrix[5] + obj.z * Matrix[9] + Matrix[13]) / Raito1 * py;
    c2d.h = (c2d.y - c2d.w) / 2;
    return true;
}

int d3dto2D(lua_State *L) {
    Addr mxaddr = luaL_checknumber(L, 1);
    float Matrix[16] = {0};

    memoryTools->readV(Matrix, 16 * 4, mxaddr);

    C3D c3D{0, 0, 0};
    c3D.x = luaL_checknumber(L, 2);
    c3D.z = luaL_checknumber(L, 3);

    c3D.x *= 0.001F;
    c3D.z *= 0.001F;

    float px = luaL_checknumber(L, 4);
    float py = luaL_checknumber(L, 5);

    C2D c2D{0, 0, 0, 0};
    m_get3Dto2D(c2D, Matrix, px, py, c3D);

    lua_pushnumber(L, c2D.x);
    lua_pushnumber(L, c2D.y);
    lua_pushnumber(L, c2D.w);
    lua_pushnumber(L, c2D.h);

    return 4;
}


int super(lua_State *L) {
    int v = luaL_checknumber(L, 1);
    int v2 = luaL_checknumber(L, 2);
    lua_pushnumber(L, v + v2);
    return 1;
}

int getSelfPid(lua_State *L) {
    lua_pushnumber(L, getpid());
    return 1;
}

int shell(lua_State *L) {
    string str = luaL_checkstring(L, 1);
    lua_pushnumber(L, system(str.c_str()));
    return 1;
}

int initToolsPm(lua_State *L) {
    string pm = luaL_checkstring(L, 1);
    memoryTools = MemoryTools::MakePm(pm.c_str());
    memoryTools->initPageMap();
    lua_pushnumber(L, memoryTools->getProcessPid());
    return 1;
}

int initToolsSelf(lua_State *L) {
    delete memoryTools;
    memoryTools = MemoryTools::MakeSELF();
    lua_pushnumber(L, memoryTools->getProcessPid());
    return 1;
}
int readL(lua_State *L) {
    long val = luaL_checklong(L, 1);
    lua_pushnumber(L, memoryTools->readL(val));
    return 1;
}
int readI(lua_State *L) {
    Addr val = luaL_checklong(L, 1);
    lua_pushnumber(L, memoryTools->readI(val));
    return 1;
}
int readF(lua_State *L) {
    Addr val = luaL_checklong(L, 1);
    lua_pushnumber(L, memoryTools->readF(val));
    return 1;
}

int readZ(lua_State *L) {
    Addr val = luaL_checklong(L, 1);
    lua_pushboolean(L, memoryTools->readZ(val));
    return 1;
}

int readV(lua_State *L) {
    long val = luaL_checklong(L, 1);
    long type = luaL_checklong(L, 2);

    if (type == MemoryTools::MEM_DWORD) {
        lua_pushinteger(L, memoryTools->readI(val));
    } else if (type == MemoryTools::MEM_QWORD) {
        lua_pushnumber(L, memoryTools->readL(val));
    } else if (type == MemoryTools::MEM_DOUBLE) {
        lua_pushnumber(L, memoryTools->readD(val));
    } else if (type == MemoryTools::MEM_FLOAT) {
        lua_pushnumber(L, memoryTools->readF(val));
    } else if (type == MemoryTools::MEM_BYTE) {
        lua_pushinteger(L, memoryTools->readI(val));
    } else {
        return 0;
    }
    return 1;
}

int writeV(lua_State *L) {
    long addr = luaL_checklong(L, 2);
    long type = luaL_checklong(L, 3);

    if (type == MemoryTools::MEM_DWORD) {
        int val = luaL_checkinteger(L, 1);
        lua_pushinteger(L, memoryTools->writeI(val, addr));
    } else if (type == MemoryTools::MEM_QWORD) {
        long val = luaL_checklong(L, 1);
        lua_pushinteger(L, memoryTools->writeL(val, addr));
    } else if (type == MemoryTools::MEM_DOUBLE) {
        double val = luaL_checknumber(L, 1);
        lua_pushinteger(L, memoryTools->writeD(val, addr));
    } else if (type == MemoryTools::MEM_FLOAT) {
        double val = luaL_checknumber(L, 1);
        lua_pushinteger(L, memoryTools->writeF(val, addr));
    } else if (type == MemoryTools::MEM_BYTE) {
        int val = luaL_checkinteger(L, 1);
        lua_pushinteger(L, memoryTools->writeB(val, addr));
    } else {
        return 0;
    }

    return 1;
}

int setRange(lua_State *L) {
    int range = luaL_checkinteger(L, 1);
    memoryTools->setSearchRange(range);
    return 1;
}

int serach(lua_State *L) {
    string val = luaL_checkstring(L, 1);
    int type = luaL_checkinteger(L, 2);
    //  printf("type:%d\n", type);
    lua_pushinteger(L, memoryTools->MemorySearch(val.c_str(), type));
    return 1;
}
int getBase(lua_State *L) {
    string baseName = luaL_checkstring(L, 1);
    printf("baseName:%s\n", baseName.c_str());
    lua_pushnumber(L, memoryTools->BaseAddr(baseName.c_str()));
    return 1;
}

int offset(lua_State *L) {
    string val = luaL_checkstring(L, 1);
    int type = luaL_checkinteger(L, 2);
    int off = luaL_checkinteger(L, 3);
    lua_pushinteger(L, memoryTools->MemoryOffset(val.c_str(), off, type));
    return 1;
}

int toHex(lua_State *L) {
    long val = luaL_checklong(L, 1);
    char buf[200];
    sprintf(buf, "%lX", val);
    lua_pushstring(L, buf);
    return 1;
}

int fHex(lua_State *L) {
    string val = luaL_checkstring(L, 1);
    return 1;

}

int arr(lua_State *L) {
    for (int i = 1; i <= 3; ++i) {
        lua_pushnumber(L, i);
        char buf[256];
        sprintf(buf, "hehe%d", i);
        lua_pushstring(L, buf);
        //        lua_settable(L, -3);
        lua_rawset(L, -3);
    }

    return 0;
}
int m_sleep(lua_State *L) {
    int time = luaL_checknumber(L, 1);
    TimeTools::sleep_ms(time);
    return 0;
}
int s_sleep(lua_State *L) {
    int time = luaL_checknumber(L, 1);
    TimeTools::sleep_s(time);
    return 0;
}

int getTime(lua_State *L) {
    lua_pushnumber(L, TimeTools::getCurrentTime());
    return 1;
}
int getPoints(lua_State *L) {
    Addr addr = luaL_checknumber(L, 1);
    int nArgs = lua_gettop(L);
    int arrCount = nArgs - 1;
    int off[arrCount];
    for (int i = 2; i <= nArgs; i++) {
        int val = luaL_checknumber(L, i);
        off[i - 2] = val;
    }
    lua_pushnumber(L, memoryTools->getPointers(addr, arrCount, off));
    return 1;
}

int readLuaArray(lua_State *L) {


    /* Addr addr = luaL_checknumber(L, 1);

     int nArgs = lua_gettop(L);
     int off[nArgs - 1];
     printf("addr:%lX\n", addr);
     for (int i = 2; i <= nArgs; i++) {
         int val = luaL_checknumber(L, i);
         printf("val:%d\n", val);
         off[i - 2] = val;
     }*/
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 2);
    lua_pushnumber(L, 3);
    lua_pushnumber(L, 4);

    //lua_pushnumber(L, memoryTools->getPointers(addr, nArgs, off));
    /*  Addr addr = luaL_checknumber(L, );
      printf("addr:%lX\n", addr);
      // lua_settop(L, 0); //这样确保我们的array是放在当前栈的栈顶。
      // lua_getglobal(L, "array");
      //如果前面不调用lua_settop(L,0),那我们必须要使用luaL_len(L,-1)
      int size = lua_objlen(L, -1);//相关于#table
      int off[size];
      memset(off, 0, size * sizeof(int));

      printf("n:%d\n", size);
      for (int i = 1; i <= size; ++i) {
          lua_pushnumber(L, i); //往栈里面压入i
          lua_gettable(L, -2); //读取table[i]，table位于-2的位置。
          int val = luaL_checknumber(L, -1);
          off[i - 1] = val;
          printf("val:%d\n", val);
          lua_pop(L, 1);
      }*/


    return 4;
}


int luaopen_module(lua_State *L) {
    static luaL_Reg mylibs[] = {{"super",         super},
                                {"getSelfPid",    getSelfPid},
                                {"shell",         shell},
                                {"initToolsPm",   initToolsPm},
                                {"initToolsSelf", initToolsSelf},
                                {"read",          readV},
                                {"readI",         readI},
                                {"readL",         readL},
                                {"readF",         readF},
                                {"readZ",         readZ},
                                {"write",         writeV},
                                {"setRange",      setRange},
                                {"serach",        serach},
                                {"offset",        offset},
                                {"getBase",       getBase},
                                {"getPoints",     getPoints},
                                {"toHex",         toHex},
                                {"sleep_ms",      m_sleep},
                                {"sleep_s",       s_sleep},
                                {"getTime",       getTime},
                                {"d3dto2D",       d3dto2D},
                                {"arr",           arr},
                                {"test",          readLuaArray},
                                {"getDataEnc",    getDataEnc},
                                {"putI",          putI},
                                {"addCrs",        addCrs},
                                {"sendData",      sendData},
                                {"sendCMD",       sendCMD},
                                {NULL, NULL}};
    //全局注册
    // lua_register(L, "super", super);
    // lib注册
    luaL_register(L, "rx", mylibs);
    return 0;
}

//注册全局变量
void lua_global(lua_State *L) {
    lua_pushnumber(L, 0);
    lua_setglobal(L, "DWORD");

    lua_pushnumber(L, 1);
    lua_setglobal(L, "QWORD");

    lua_pushnumber(L, 2);
    lua_setglobal(L, "FLOAT");

    lua_pushnumber(L, 3);
    lua_setglobal(L, "DOUBLE");

    lua_pushnumber(L, 4);
    lua_setglobal(L, "BYTE");

    lua_pushnumber(L, 0x1);
    lua_setglobal(L, "ALL");

    lua_pushnumber(L, 0x2);
    lua_setglobal(L, "B_BAD");

    lua_pushnumber(L, 0x8);
    lua_setglobal(L, "C_BSS");

    lua_pushnumber(L, 0x10);
    lua_setglobal(L, "C_DATA");

    lua_pushnumber(L, 0x20);
    lua_setglobal(L, "C_HEAP");

    lua_pushnumber(L, 0x40);
    lua_setglobal(L, "JAVA_HEAP");

    lua_pushnumber(L, 0x80);
    lua_setglobal(L, "A_ANONMYOUS");

    lua_pushnumber(L, 0x100);
    lua_setglobal(L, "CODE_SYSTEM");

    lua_pushnumber(L, 0x200);
    lua_setglobal(L, "STACK");

    lua_pushnumber(L, 0x400);
    lua_setglobal(L, "ASHMEM");

    lua_pushnumber(L, 2340);
    lua_setglobal(L, "MX");

    lua_pushnumber(L, 1080);
    lua_setglobal(L, "MY");
}

void lua_global_cmd(lua_State *L) {
    lua_pushnumber(L, 11000);
    lua_setglobal(L, "D_DATA");

    lua_pushnumber(L, 11001);
    lua_setglobal(L, "D_GAME_VOER");

    lua_pushnumber(L, 11002);
    lua_setglobal(L, "D_BLUE");

    lua_pushnumber(L, 11003);
    lua_setglobal(L, "D_RED");

    lua_pushnumber(L, 11004);
    lua_setglobal(L, "D_TEST");

    lua_pushnumber(L, 11005);
    lua_setglobal(L, "D_CLEAR");

    lua_pushnumber(L, 11006);
    lua_setglobal(L, "D_HEAHER");

    lua_pushnumber(L, 11007);
    lua_setglobal(L, "D_MESSGE");

    lua_pushnumber(L, 11008);
    lua_setglobal(L, "D_LOG");

    lua_pushnumber(L, 11014);
    lua_setglobal(L, "D_PROISNOT");

    lua_pushnumber(L, 11015);
    lua_setglobal(L, "D_SHARE");

    lua_pushnumber(L, 11016);
    lua_setglobal(L, "D_FLAT_VIS");


}

void lua_main(char *lua) {

    int error, error1, error2;
    g_L = lua_open();
    luaopen_module(g_L);
    lua_global(g_L);
    lua_global_cmd(g_L);
    luaL_openlibs(g_L);
//    error1 = luaL_loadfile(g_L, "/data/local/tmp/test.lua");
//    error1 = luaL_loadstring(g_L, "print(\"Lua init\")");
    error1 = luaL_loadstring(g_L, lua);
    error2 = lua_pcall(g_L, 0, 0, 0);
    error = error1 || error2;
    if (error) {
        fprintf(stderr, "%s\n", lua_tostring(g_L, -1));
        lua_pop(g_L, 1);
    }
    lua_close(g_L);
    delete[] lua;
}
}
