#include <list>
#include "MemoryTools.h"
#include "TimeTools.h"
#include <unistd.h>


int main() {
    //内存工具
    MemoryTools *memTools = MemoryTools::MakePm("com.aaaaaaaaaaaa");
    memTools->initPageMap();

    Addr add1 = 0x89565660;
    int range = 0x2500;
    Addr off = 0x000;
    int *values = new int[range / 4];
    bool isLoad = false;

    while (1) {
        for (int i = 0; i < range; i++) {
            int value = memTools->readI(add1, i * 4);
            if (!isLoad) {
                if (value == 0) {
                    values[i] = 1;
                }
            } else {
                if (values[i] == 1) {
                    if (value == 4)
                        printf("off:%5lX value:%d\n", i * 4, value);
                }
            }

        }
        isLoad = true;
//        printf("\r");
//        sleep(1);
        TimeTools::sleep_ms(100);
        printf("\x1b[H\x1b[2J");
    }

}
