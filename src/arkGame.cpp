#include <MemoryTools.h>
#include <GameTools.h>
#include <draw.h>
#include <touch.h>

/**
 * 方舟读取
 */


/**
 * 判断是否是玩家
 */
bool isplayer(char *name) {
    // 对象名称
    if (strstr(name, "wnTest_Female_C")) {
        return true;
    }
    if (strstr(name, "wnTest_Male_C")) {
        return true;
    }
    return false;
}

int main() {
    if (!initDraw(true)) {
        exit(0);
    }
    Init_touch_config();

    MemoryTools *memTools = MemoryTools::MakePm("com.studiowildcard.wardrumstudios.ark");
    auto *gameTools = new GameTools(memTools);

    printf("Game Pid: %d\n", memTools->getPid());
    Addr BaseAddr = memTools->BaseAddr("libUE4.so");
    printf("BaseAddr: %lx\n", BaseAddr);
    Addr Gname = memTools->readAddr(BaseAddr, 0x74F0480);
    printf("Gname: %lx\n", Gname);
    Addr GWorld = memTools->readAddr(BaseAddr, 0x762F4B8);
    Addr a = memTools->readAddr(BaseAddr, 0x760A1A0);
    printf("GWorld: %lx\n", GWorld);
    Addr MxAddr = memTools->readAddr(a + 0x20) + 0x290;
    printf("MxAddr: %lx\n", MxAddr);
    Addr uleve = memTools->readAddr(GWorld, 0x58);

    float matrix[16] = {0}; // 矩阵数据
    float px = 1200 - (60 / 2);
    float py = 640;
    bool flag = BaseAddr > 0;
    while (flag) {
        Addr ActorArray = memTools->readAddr(uleve, 0xA0);
        int count = memTools->readI(uleve, 0xA8);
        drawBegin();

        char buff[256];
        char name[32];
        for (int i = 0; i < count; i++) {
            Addr Object = memTools->readAddr(ActorArray, 8 * i);
            Addr ActorPosition = memTools->readAddr(Object, 0x2C8);
            int oid = memTools->readI(Object, 0x18);
            Addr HPAdddr = memTools->readAddr(Object, 0x1068);
            float pw = memTools->readF(HPAdddr, 0x964);
            float hp = memTools->readF(HPAdddr, 0x960);
            if (hp <= 0) {
                continue;
            }
            memset(name, 0, 0x20);
            // 读取物品名称
            gameTools->getObjName(oid, Gname, 0x20, name);
//            if (!isplayer(name)) {
//                continue;
//            }
//            memcpy(name, name + 2, 30);
            C3D ObjCoor{};
            memTools->readV(&ObjCoor, 0xC, ActorPosition, 0x2D0);
            sprintf(buff, "Object:%lX pw:%f hp:%f", Object, pw, hp);
            C2D o2d{};
            memTools->readV(matrix, 4 * 16, MxAddr);
            if (!GameTools::get3Dto2D(o2d, matrix, px, py, ObjCoor)) {
                continue;
            }

            float fx = 60;
            float fy = -60;

            ImDrawList *draw = ImGui::GetForegroundDrawList();
            draw->AddText(ImVec2(o2d.x - o2d.w / 2, o2d.y - o2d.h), IM_COL32(255, 0, 0, 255),
                          name);


            float fbx = o2d.x + fx;
            float fby = o2d.y + fy;

            ImU32 color = IM_COL32(255, 0, 0, 255);
            // 画方框
            draw->AddLine(ImVec2(fbx - (o2d.w / 3) / 2, fby - o2d.h),
                          ImVec2(fbx - o2d.w, fby - o2d.h),

                          color);
            draw->AddLine(ImVec2(fbx - o2d.w, fby - o2d.h),
                          ImVec2(fbx - o2d.w, fby - o2d.h / 2),
                          color);

            draw->AddLine(ImVec2(fbx + (o2d.w / 3) / 2, fby - o2d.h),
                          ImVec2(fbx + o2d.w, fby - o2d.h),
                          color);
            draw->AddLine(ImVec2(fbx + o2d.w, fby - o2d.h),
                          ImVec2(fbx + o2d.w, fby - o2d.h / 2),
                          color);

            draw->AddLine(ImVec2(fbx - (o2d.w / 3) / 2, fby + o2d.h),
                          ImVec2(fbx - o2d.w, fby + o2d.h),
                          color);
            draw->AddLine(ImVec2(fbx - o2d.w, fby + o2d.h),
                          ImVec2(fbx - o2d.w, fby + o2d.h / 2),
                          color);

            draw->AddLine(ImVec2(fbx + (o2d.w / 3) / 2, fby + o2d.h),
                          ImVec2(fbx + o2d.w, fby + o2d.h),
                          color);
            draw->AddLine(ImVec2(fbx + o2d.w, fby + o2d.h),
                          ImVec2(fbx + o2d.w, fby + o2d.h / 2),
                          color);

        }
        drawEnd();
    }

    shutdown();
    return 0;
}