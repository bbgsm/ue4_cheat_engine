#include <stdbool.h>

#ifndef GNAME_UTILS
#define GNAME_UTILS

int m_state = -1;

bool isplayer(char *name)
{
    // 对象名称
    if (strstr(name, "Player")) {
        m_state = 5;
        return true;
    }
    return false;
}


int qlx(char *name) {

    return 0;
}


int qsc(char *name) {

    return 0;
}

#endif


