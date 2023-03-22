
#include <cstring>
#include "GameTools.h"
#include "MemoryTools.h"
#include "abiUtil.h"

//临时变量
GameTools::GameTools(MemoryTools *memTools) {
    m_MemTools = memTools;
}

/**
 * 计算三维物体在屏幕的坐标
 * @param matrix 矩阵16x16
 * @param px 屏幕宽度像素除以2
 * @param py 屏幕像素高度除以2
 * @param obj 坐标结构体
 * @return  2D 坐标结构体
 */

bool GameTools::get3Dto2D(C2D &o2d, const float *matrix, const float px, const float py, struct C3D &obj) {
    float camear_z = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    //判断人物是否在后方
    float r_x = px + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear_z * px;
    float r_y = py - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * (obj.z - 15) + matrix[13]) / camear_z * py;
    float r_w = py - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * (obj.z + 205) + matrix[13]) / camear_z * py;

    o2d.x = r_x;
    o2d.y = r_y;
    o2d.w = ((r_y - r_w) / 2.0F) / 2.5F;
    o2d.h = (r_y - r_w) / 2.3F;

    if (camear_z < 0.01F) {
        o2d.y = py * 2.0F + 100.0F;
        o2d.x = px * 2.0F - o2d.x;
        return false;
    }

    return true;
}


void GameTools::ToMatrixWithScale(D3DXMATRIX &dxmatrix, D3DXVECTOR4 &Rotation, D3DVector &Translation,
                                  D3DVector &Scale3D) {

    float X2, Y2, Z2, xX2, Yy2, Zz2, Zy2, Wx2, Xy2, Wz2, Zx2, Wy2;

    dxmatrix._41 = Translation.X;
    dxmatrix._42 = Translation.Y;
    dxmatrix._43 = Translation.Z;

    X2 = Rotation.X + Rotation.X;
    Y2 = Rotation.Y + Rotation.Y;
    Z2 = Rotation.Z + Rotation.Z;

    xX2 = Rotation.X * X2;
    Yy2 = Rotation.Y * Y2;
    Zz2 = Rotation.Z * Z2;

    dxmatrix._11 = (1 - (Yy2 + Zz2)) * Scale3D.X;
    dxmatrix._22 = (1 - (xX2 + Zz2)) * Scale3D.Y;
    dxmatrix._33 = (1 - (xX2 + Yy2)) * Scale3D.Z;

    Zy2 = Rotation.Y * Z2;
    Wx2 = Rotation.W * X2;

    dxmatrix._32 = (Zy2 - Wx2) * Scale3D.Z;
    dxmatrix._23 = (Zy2 + Wx2) * Scale3D.Y;

    Xy2 = Rotation.X * Y2;
    Wz2 = Rotation.W * Z2;
    dxmatrix._21 = (Xy2 - Wz2) * Scale3D.Y;
    dxmatrix._12 = (Xy2 + Wz2) * Scale3D.X;


    Zx2 = Rotation.X * Z2;
    Wy2 = Rotation.W * Y2;

    dxmatrix._31 = (Zx2 + Wy2) * Scale3D.Z;
    dxmatrix._13 = (Zx2 - Wy2) * Scale3D.X;

    dxmatrix._14 = 0;
    dxmatrix._24 = 0;
    dxmatrix._34 = 0;
    dxmatrix._44 = 1;
}


void GameTools::D3DXMatrixMultiply(D3DXMATRIX &Pout, D3DXMATRIX &Pm1, D3DXMATRIX &Pm2) {
    Pout._11 = Pm1._11 * Pm2._11 + Pm1._12 * Pm2._21 + Pm1._13 * Pm2._31 + Pm1._14 * Pm2._41;
    Pout._12 = Pm1._11 * Pm2._12 + Pm1._12 * Pm2._22 + Pm1._13 * Pm2._32 + Pm1._14 * Pm2._42;
    Pout._13 = Pm1._11 * Pm2._13 + Pm1._12 * Pm2._23 + Pm1._13 * Pm2._33 + Pm1._14 * Pm2._43;
    Pout._14 = Pm1._11 * Pm2._14 + Pm1._12 * Pm2._24 + Pm1._13 * Pm2._34 + Pm1._14 * Pm2._44;
    Pout._21 = Pm1._21 * Pm2._11 + Pm1._22 * Pm2._21 + Pm1._23 * Pm2._31 + Pm1._24 * Pm2._41;
    Pout._22 = Pm1._21 * Pm2._12 + Pm1._22 * Pm2._22 + Pm1._23 * Pm2._32 + Pm1._24 * Pm2._42;
    Pout._23 = Pm1._21 * Pm2._13 + Pm1._22 * Pm2._23 + Pm1._23 * Pm2._33 + Pm1._24 * Pm2._43;
    Pout._24 = Pm1._21 * Pm2._14 + Pm1._22 * Pm2._24 + Pm1._23 * Pm2._34 + Pm1._24 * Pm2._44;
    Pout._31 = Pm1._31 * Pm2._11 + Pm1._32 * Pm2._21 + Pm1._33 * Pm2._31 + Pm1._34 * Pm2._41;
    Pout._32 = Pm1._31 * Pm2._12 + Pm1._32 * Pm2._22 + Pm1._33 * Pm2._32 + Pm1._34 * Pm2._42;
    Pout._33 = Pm1._31 * Pm2._13 + Pm1._32 * Pm2._23 + Pm1._33 * Pm2._33 + Pm1._34 * Pm2._43;
    Pout._34 = Pm1._31 * Pm2._14 + Pm1._32 * Pm2._24 + Pm1._33 * Pm2._34 + Pm1._34 * Pm2._44;
    Pout._41 = Pm1._41 * Pm2._11 + Pm1._42 * Pm2._21 + Pm1._43 * Pm2._31 + Pm1._44 * Pm2._41;
    Pout._42 = Pm1._41 * Pm2._12 + Pm1._42 * Pm2._22 + Pm1._43 * Pm2._32 + Pm1._44 * Pm2._42;
    Pout._43 = Pm1._41 * Pm2._13 + Pm1._42 * Pm2._23 + Pm1._43 * Pm2._33 + Pm1._44 * Pm2._43;
    Pout._44 = Pm1._41 * Pm2._14 + Pm1._42 * Pm2._24 + Pm1._43 * Pm2._34 + Pm1._44 * Pm2._44;
}

D3DXMATRIX Bone_Matrix;
D3DXMATRIX Component_ToWorld_Matrix;
D3DXMATRIX New_Matrix;
FTransform Bone;

void GameTools::Get_Bone_3D(C3D &c3D, Addr Mesh_Offset, FTransform &Actor, int Part) {
    // 骨骼指针
    Addr Bone_Address = m_MemTools->readAddr(Mesh_Offset) + 0x30;
    ReadFTransform(Bone, Bone_Address + Part * 0x30);
    // 获取Actor数据
    ToMatrixWithScale(Bone_Matrix, Bone.Rotation, Bone.Translation, Bone.Scale3D);
    ToMatrixWithScale(Component_ToWorld_Matrix, Actor.Rotation, Actor.Translation, Actor.Scale3D);
    D3DXMatrixMultiply(New_Matrix, Bone_Matrix, Component_ToWorld_Matrix);
    c3D.x = New_Matrix._41;
    c3D.y = New_Matrix._42;
    c3D.z = New_Matrix._43;
}

/**
 * 计算三维物体在屏幕的坐标
 * @param matrix 矩阵16x16
 * @param px 屏幕宽度像素除以2
 * @param py 屏幕像素高度除以2
 * @param obj 坐标结构体
 * @return  2D 坐标结构体
 */
void GameTools::getBone3Dto2D(C2D &d2D, const float *matrix, const int x, const int y, C3D &obj) {
    float camear_z = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    if (camear_z < 0.01) {
        memset(&d2D, 0, sizeof(d2D));
        return;
    }
    camear_z = 1.0F / camear_z;
    auto px = (float) x;
    auto py = (float) y;

    float r_x = px + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) * camear_z * px;
    float r_y = py - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) * camear_z * py;
    float r_w = py - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * (obj.z + 205) + matrix[13]) * camear_z * py;

    d2D.x = r_x;
    d2D.y = r_y;
    d2D.w = ((r_y - r_w) / 2) / 2;
    d2D.h = (r_y - r_w) / 2;
}

/**
 * 获取对象名称
 * @param objAddr 名称地址
 * @param temp  名称字符数组
 * @param i 名称长度
 */
void GameTools::getPlayerName(Addr objAddr, char *buff, int *len) {
    Addr nameAddr = m_MemTools->readAddr(objAddr);
    int count = m_MemTools->readI(objAddr + sizeof(Addr));
    m_MemTools->readV(buff, count * 2, nameAddr);
    *len = count * 2;
}

Bone3D hero3D;

void GameTools::Get_Bone_2D(Bone2D &hero2D, float *matrix, int px, int py, Addr Mesh_Offset) {
    //获取3D骨骼
    Get_Hero_Bone_3D(hero3D, Mesh_Offset);
    getBone3Dto2D(hero3D, hero2D, matrix, px, py);
}

FTransform Actor;

void GameTools::ReadFTransform(FTransform &transform, Addr location) {
    m_MemTools->readV(&transform, 0x1C, location);
//    m_MemTools->readV(&transform.Rotation, sizeof(transform.Rotation), location);
//    m_MemTools->readV(&transform.Translation, sizeof(transform.Translation), location + 0x10);
    m_MemTools->readV(&transform.Scale3D, sizeof(transform.Scale3D), location, 0x20);
}

void GameTools::ReadFTransform1(FTransform &transform, Addr location) {
    transform.Rotation.X = m_MemTools->readF(location);
    transform.Rotation.Y = m_MemTools->readF(location + 4);
    transform.Rotation.Z = m_MemTools->readF(location + 8);
    transform.Rotation.W = m_MemTools->readF(location + 12);
    transform.Translation.X = m_MemTools->readF(location + 16);
    transform.Translation.Y = m_MemTools->readF(location + 20);
    transform.Translation.Z = m_MemTools->readF(location + 24);
    transform.Scale3D.X = m_MemTools->readF(location + 32);
    transform.Scale3D.Y = m_MemTools->readF(location + 36);
    transform.Scale3D.Z = m_MemTools->readF(location + 40);
}

/**
 * 读取玩家骨骼信息
 * @param h3D 接收的骨骼3d信息
 * @param Mesh_Offset Mesh偏移
 */
void GameTools::Get_Hero_Bone_3D(Bone3D &h3D, Addr Mesh_Offset) {

//    Addr Bone_Offset = Mesh_Offset + 0x6E0;
    Addr Bone_Offset = Mesh_Offset + abiUtil::abi(0x590, 0x6D0);
    //人类地址
    ReadFTransform(Actor, Mesh_Offset + abiUtil::abi(0x150, 0x1B0));
    Get_Bone_3D(h3D.Head, Bone_Offset, Actor, 5);
    Get_Bone_3D(h3D.Pit, Bone_Offset, Actor, 4);
    Get_Bone_3D(h3D.Pelvis, Bone_Offset, Actor, 1);
    Get_Bone_3D(h3D.Lcollar, Bone_Offset, Actor, 11);
    Get_Bone_3D(h3D.Rcollar, Bone_Offset, Actor, 32);
    Get_Bone_3D(h3D.Lelbow, Bone_Offset, Actor, 12);
    Get_Bone_3D(h3D.Relbow, Bone_Offset, Actor, 33);
    Get_Bone_3D(h3D.Lwrist, Bone_Offset, Actor, 63);
    Get_Bone_3D(h3D.Rwrist, Bone_Offset, Actor, 62);
    Get_Bone_3D(h3D.Lthigh, Bone_Offset, Actor, 52);
    Get_Bone_3D(h3D.Rthigh, Bone_Offset, Actor, 56);
    Get_Bone_3D(h3D.Lknee, Bone_Offset, Actor, 53);
    Get_Bone_3D(h3D.Rknee, Bone_Offset, Actor, 57);
    Get_Bone_3D(h3D.Lankle, Bone_Offset, Actor, 54);
    Get_Bone_3D(h3D.Rankle, Bone_Offset, Actor, 58);
}

/**
 * 骨骼3d坐标转屏幕坐标
 * @param h3D  3d坐标
 * @param hero2D 接收的屏幕坐标
 * @param matrix 矩阵
 * @param px 屏幕宽 / 2
 * @param py 屏幕高 / 2
 */
void GameTools::getBone3Dto2D(Bone3D &h3D, Bone2D &hero2D, float *matrix, int px, int py) {
    getBone3Dto2D(hero2D.Head, matrix, px, py, h3D.Head);
    getBone3Dto2D(hero2D.Pit, matrix, px, py, h3D.Pit);
    getBone3Dto2D(hero2D.Pelvis, matrix, px, py, h3D.Pelvis);
    getBone3Dto2D(hero2D.Lcollar, matrix, px, py, h3D.Lcollar);
    getBone3Dto2D(hero2D.Rcollar, matrix, px, py, h3D.Rcollar);
    getBone3Dto2D(hero2D.Lelbow, matrix, px, py, h3D.Lelbow);
    getBone3Dto2D(hero2D.Relbow, matrix, px, py, h3D.Relbow);
    getBone3Dto2D(hero2D.Lwrist, matrix, px, py, h3D.Lwrist);
    getBone3Dto2D(hero2D.Rwrist, matrix, px, py, h3D.Rwrist);
    getBone3Dto2D(hero2D.Lthigh, matrix, px, py, h3D.Lthigh);
    getBone3Dto2D(hero2D.Rthigh, matrix, px, py, h3D.Rthigh);
    getBone3Dto2D(hero2D.Lknee, matrix, px, py, h3D.Lknee);
    getBone3Dto2D(hero2D.Rknee, matrix, px, py, h3D.Rknee);
    getBone3Dto2D(hero2D.Lankle, matrix, px, py, h3D.Lankle);
    getBone3Dto2D(hero2D.Rankle, matrix, px, py, h3D.Rankle);
}

/**
 * UE4 4.23读取对象名称
 */
void GameTools::getObjName(int id, Addr Gname, int len, char *buffer) {
    if (id > 0 && id < 2000000) {
        // 块
        int block = id / 16384;
        // 偏移
        int offset = id % 16384;
        Addr namePoolChunk = m_MemTools->readAddr(Gname + block * sizeof(Addr));
        Addr fNameEntry = m_MemTools->readAddr(namePoolChunk + offset * sizeof(Addr));
        m_MemTools->readV(buffer, len, fNameEntry, 0x10 + sizeof(Addr));
    }
}

/**
 * UE4 4.23读取对象名称
 */
void GameTools::getObjName4_23(int id, Addr Gname, int len, char *buffer) {
    if (id > 0 && id < 2000000) {
        // 块
        int block = id >> 16;
        // 偏移
        int offset = id & 65535;
        Addr namePoolChunk = m_MemTools->readAddr(Gname + abiUtil::abi(0xC, 0x10) + block * sizeof(Addr));
        Addr fNameEntry = namePoolChunk + (2 * offset);
        int16 fNameEntryHeader = m_MemTools->readI16(fNameEntry);
        int strLength = fNameEntryHeader >> 6;
        if (strLength > 0 && strLength < 250) {
            m_MemTools->readV(buffer, strLength, fNameEntry);
        }
    }
}












