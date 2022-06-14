

#include <ctime>
#include <cstring>
#include <unistd.h>
#include "TimeTools.h"


void TimeTools::getFormatTime(char *str_time) //获取格式化时间
{
    time_t now;
    struct tm *tm_now;
    char datetime[128];
    time(&now);
    tm_now = localtime(&now);
    strftime(datetime, 128, "%Y-%m-%d %H:%M:%S", tm_now);
    strcpy(str_time, datetime);
}

void TimeTools::sleep_s(int s) {
    ::sleep(s);
}

/* 毫秒级 延时 */
void TimeTools::sleep_ms(int ms) {
    struct timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = ms * 1000;    //
    select(0, nullptr, nullptr, nullptr, &delay);
}

/* 微妙级 延时 */
void TimeTools::sleep_us(int us) {
    struct timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = us;
    select(0, nullptr, nullptr, nullptr, &delay);
}

//获取系统时间
mlong TimeTools::getCurrentTime() {
    timeval tv{};
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

