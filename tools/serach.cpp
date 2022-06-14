
#include <list>
#include "MemoryTools.h"
#include "GameTools.h"
#include "GnameUtils.h"


void mm() {



    GameTools *gameTools = new GameTools(memTools);


   MemoryTools *memTools = MemoryTools::MakePm("");

     memTools->MemorySearch("1065353216",  DWORD);
     memTools->MemoryOffset("1065353216", 4,  DWORD);
     memTools->MemoryOffset("1065353216", 8,  DWORD);
     memTools->MemoryOffset("1065353216", 12,  DWORD);
     memTools->MemoryOffset("0", 16,  DWORD);
     memTools->MemoryOffset("1065353216", 24,  DWORD);
     memTools->MemoryOffset("0", 28,  DWORD);
     memTools->MemoryOffset("0", 32,  DWORD);
     memTools->MemoryOffset("1065353216", 36,  DWORD);
     memTools->MemoryOffset("3", 264,  FLOAT);

   for (list<RADDR>::iterator pmapsit = memTools->getResults()->begin(); pmapsit != memTools->getResults()->end(); ++pmapsit) {
         printf("addr:%lX\n", pmapsit->addr);
    }

//=================================

    Addr baseAddr = memTools->BaseAddr("");
    Addr coorBase = baseAddr + 0x75A72A4;
    Addr UWorld = memTools->readUL(coorBase);
    Addr Gname = memTools->readUL(baseAddr + 0x73C6094);//Gname

    printf("UWorld:%X\n", UWorld);
    printf("Gname:%X\n", Gname);




}
