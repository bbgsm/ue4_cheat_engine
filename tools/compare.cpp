
#include <list>
#include "MemoryTools.h"


int main(int argv, char **argc) {

    if (argv != 4) {
        return puts("参数错误");
    }

    Addr add1 = 0x7C58411000;
    Addr add2 = 0x7C3AFAE000;
    Addr add3 = 0x7C4FFCC000;

    sscanf(argc[1], "%x", &add1);
    sscanf(argc[2], "%x", &add2);
    sscanf(argc[3], "%x", &add3);
    // Addr add4 = 0x825BECC0;

    int range = 0x2100;

    int *arr1 = new int[range / 4];
    int *arr2 = new int[range / 4];
    int *arr3 = new int[range / 4];
    // Addr *arr4 = new Addr[range];


    //内存工具
    MemoryTools *memTools = MemoryTools::MakePm("com.aaaaaaaaa.bbbbbbbb");
    memTools->initPageMap();

    memTools->readV(arr1, range, add1);
    memTools->readV(arr2, range, add2);
    memTools->readV(arr3, range, add3);
    // memTools->readV(arr4, range, add4);

    bool temp;
    for (int i = 0; i < (range / 4); i++) {
        temp = false;
        if (arr1[i] != arr2[i] && (arr2[i] == arr3[i])) {
            printf("偏移:%lX  %lX --- %15d  %lX --- %15d   %lX --- %15d  \n",
                   i * sizeof(int), add1 + i * sizeof(int), arr1[i],
                   add2 + i * sizeof(int), arr2[i],
                   add3 + i * sizeof(int), arr3[i]/*,
                   add4 + i * sizeof(Addr), arr4[i]*/);
        }
    }

}
