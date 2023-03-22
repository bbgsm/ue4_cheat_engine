/**
 * 自动点击任务
 */

#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

#include "TimeTools.h"
#include "eventTouch.h"

using namespace std;

void click(int x, int y) {
    touchDown(x, y, 0);
    TimeTools::sleep_ms(200);
    touchUp(0);
}

void move(int x, int y) {
    touchMove(x, y, 0);
}

void longClick(int x, int y, int s) {
    touchDown(x, y, 0);
    TimeTools::sleep_s(s);
    touchUp(0);
}

// Android KeyEvent模拟及KeyCode原生代码对照表
// https://blog.csdn.net/u010871962/article/details/120657210
/**
 * 打开屏幕
 */
void turnOnScreen() {
    system("input keyevent 224");
}

/**
 * home键
 */
void home() {
    system("input keyevent 3");
}

/**
 * back键
 */
void back() {
    system("input keyevent 4");
}

void log(const string &log) {
    time_t now = time(nullptr);
    tm *tm_t = localtime(&now);
    std::stringstream ss;
    ss << tm_t->tm_year + 1900 << "-" << setw(2) << setfill('0') << tm_t->tm_mon + 1 << "-" << setw(2) << setfill('0')
       << tm_t->tm_mday
       << " " << setw(2) << setfill('0') << tm_t->tm_hour << ":" << setw(2) << setfill('0') << tm_t->tm_min << ":"
       << setw(2) << setfill('0') << tm_t->tm_sec;
    cout << ss.str() << "  " << log << endl;
}

int main() {
    bool flag = false;
    while (true) {
        time_t now = time(nullptr);
        tm *tm_t = localtime(&now);
        // 判断在早上8点的时候执行
        if (tm_t->tm_hour == 8 && tm_t->tm_min == 0) {
            if (flag) {
                initTouch();
                log("开始任务......");
                turnOnScreen();
                TimeTools::sleep_s(1);
                // 打开APP
                // system("am start 软件包名/需要启动的Activity类路径");
                TimeTools::sleep_s(6);
                click(550, 2000);
                TimeTools::sleep_s(2);
                click(550, 2000);
                TimeTools::sleep_s(2);
                click(550, 2000);

                TimeTools::sleep_s(1);
                longClick(140, 900, 3);
                TimeTools::sleep_s(6);

                back();
                TimeTools::sleep_ms(300);
                back();

                closeTouch();
                log("收集完成!");
                flag = false;

            }
        } else {
            flag = true;
        }
        // 延时20秒
        TimeTools::sleep_s(20);
    }

    return 0;
}
