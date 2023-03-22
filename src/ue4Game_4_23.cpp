#include <MemoryTools.h>
#include <GameTools.h>
#include <draw.h>
#include <touch.h>
#include <android/log.h>

/**
 * ue4 4.23读取测试
 */


int main() {
    if (!initDraw(true)) {
        exit(0);
    }
    Init_touch_config();

    MemoryTools *memTools = MemoryTools::MakePm("com.ga.gg");
    auto *gameTools = new GameTools(memTools);

    printf("Game Pid: %d\n", memTools->getPid());
    Addr BaseAddr = memTools->BaseAddr("libUE4.so");
    printf("BaseAddr: %lx\n", BaseAddr);
    Addr Gname = BaseAddr + 0xA40FF40 + 0x30;
    printf("Gname: %lx\n", Gname);

    Addr GWorld = memTools->readAddr(BaseAddr, 0x0A5707B0);
    Addr a = memTools->readAddr(BaseAddr, 0xA5409C0);
    printf("GWorld: %lx\n", GWorld);
    Addr MxAddr = memTools->readAddr(a + 0x20) + 0x270;
    printf("MxAddr: %lx\n", MxAddr);

    Addr uleve = memTools->readAddr(GWorld, 0x38);


    float matrix[16] = {0}; // 矩阵数据
    float px = displayInfo.width / 2;
    float py = displayInfo.height / 2;
    bool flag = BaseAddr > 0;
    while (flag) {
        Addr ActorArray = memTools->readAddr(uleve, 0xA0);
        int count = memTools->readI(uleve, 0xA8);
        drawBegin();

        char buff[256];
        char name[256];

        for (int i = 0; i < count; i++) {
            Addr Object = memTools->readAddr(ActorArray, 8 * i);
            Addr ActorPosition = memTools->readAddr(Object, 0x248);
            int oid = memTools->readI(Object, 0x18);

            memset(name, 0, 0x20);
            // ue4 4.23 读取物品名称
            gameTools->getObjName4_23(oid, Gname, 0x20, name);
            memcpy(name, name + 2, 30);
            sprintf(buff, "%d %s", oid, name);
            C3D ObjCoor{};
            memTools->readV(&ObjCoor, 0xC, ActorPosition, 0x1E0);

            C2D o2d{};
            memTools->readV(matrix, 4 * 16, MxAddr);
            if (!GameTools::get3Dto2D(o2d, matrix, px, py, ObjCoor)) {
                continue;
            }

            float fx = 0;
            float fy = 0;

            ImDrawList *draw = ImGui::GetForegroundDrawList();
            draw->AddText(ImVec2(o2d.x - o2d.w / 2, o2d.y - o2d.h), IM_COL32(255, 0, 0, 255),
                          buff);

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
//        flag = false;
    }

    shutdown();
    return 0;
}