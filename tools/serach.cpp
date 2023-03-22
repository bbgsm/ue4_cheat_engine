
#include <list>
#include "MemoryTools.h"
#include "GameTools.h"
#include "GnameUtils.h"


int main() {
    MemoryTools *memTools = MemoryTools::MakePm("com.ga.gg");
    memTools->initPageMap();
    memTools->setSearchRange(MemoryTools::C_ALLOC);

    Addr BaseAddr = memTools->BaseAddr("libUE4.so");
    printf("BaseAddr: %lx\n", BaseAddr);
    printf("Gname: %lx\n", (BaseAddr + 0xA40FF80));
    printf("GWorld: %lx\n", BaseAddr + 0x0A5707B0);


    printf("pid:%d\n", memTools->getPid());
    int c = memTools->MemorySearch("117637123", MemoryTools::MEM_DWORD);
    printf("MemorySearch:%d\n", c);
    c = memTools->MemoryOffset("126780", 4, MemoryTools::MEM_DWORD);
    printf("MemorySearch:%d\n", c);

//    c = memTools->MemoryOffset("-572662307", 8, MemoryTools::MEM_DWORD);
//    printf("MemorySearch:%d\n", c);

    //    c = memTools->MemoryOffset("15", 12, MemoryTools::MEM_DWORD);
    //    printf("MemorySearch:%d\n", c);

    for (list<RADDR>::iterator pmapsit = memTools->getResults()->begin();
         pmapsit != memTools->getResults()->end(); ++pmapsit) {
        printf("addr:%lX\n", pmapsit->addr);
    }


}
