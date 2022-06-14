#include "Type.h"
#include "MemoryTools.h"

#pragma once


struct D3DXVECTOR4 {
    float X;
    float Y;
    float Z;
    float W;
};

struct D3DVector {
    float X;
    float Y;
    float Z;
};


struct FTransform {
    struct D3DXVECTOR4 Rotation;
    struct D3DVector Translation;
    struct D3DVector Scale3D;
};

struct D3DXMATRIX {
    float _11;
    float _12;
    float _13;
    float _14;
    float _21;
    float _22;
    float _23;
    float _24;
    float _31;
    float _32;
    float _33;
    float _34;
    float _41;
    float _42;
    float _43;
    float _44;
};
struct Bone2D {
    C2D Head{0, 0, 0, 0};             //头部
    C2D Pit{0, 0, 0, 0};              //胸部
    C2D Pelvis{0, 0, 0, 0};           //骨盆
    C2D Lcollar{0, 0, 0, 0};          //左肩膀
    C2D Rcollar{0, 0, 0, 0};          //右肩膀
    C2D Lelbow{0, 0, 0, 0};           //左手肘
    C2D Relbow{0, 0, 0, 0};           //右手肘
    C2D Lwrist{0, 0, 0, 0};           //左手腕
    C2D Rwrist{0, 0, 0, 0};           //右手腕
    C2D Lthigh{0, 0, 0, 0};           //左大腿
    C2D Rthigh{0, 0, 0, 0};           //右大腿
    C2D Lknee{0, 0, 0, 0};            //左膝盖
    C2D Rknee{0, 0, 0, 0};            //右膝盖
    C2D Lankle{0, 0, 0, 0};           //左脚腕
    C2D Rankle{0, 0, 0, 0};           //右脚腕
};

struct Bone3D {
    C3D Head{0, 0, 0};             //头部
    C3D Pit{0, 0, 0};              //胸部
    C3D Pelvis{0, 0, 0};           //骨盆
    C3D Lcollar{0, 0, 0};          //左肩膀
    C3D Rcollar{0, 0, 0};          //右肩膀
    C3D Lelbow{0, 0, 0};           //左手肘
    C3D Relbow{0, 0, 0};           //右手肘
    C3D Lwrist{0, 0, 0};           //左手腕
    C3D Rwrist{0, 0, 0};           //右手腕
    C3D Lthigh{0, 0, 0};           //左大腿
    C3D Rthigh{0, 0, 0};           //右大腿
    C3D Lknee{0, 0, 0};            //左膝盖
    C3D Rknee{0, 0, 0};            //右膝盖
    C3D Lankle{0, 0, 0};           //左脚腕
    C3D Rankle{0, 0, 0};           //右脚腕
};


class GameTools {

private:
    MemoryTools *m_MemTools;

public:
    GameTools(MemoryTools *memTools);

    static bool get3Dto2D(C2D &o2d, const float *matrix, const float px, const float py, struct C3D &obj);

    void ReadFTransform(FTransform &fTransform, Addr location);
    void ReadFTransform1(FTransform &fTransform, Addr location);

    static void ToMatrixWithScale(D3DXMATRIX &dxmatrix, D3DXVECTOR4 &Rotation, D3DVector &Translation, D3DVector &Scale3D);

    void Get_Bone_3D(C3D &c3D, Addr Mesh_Offset, FTransform &Actor, int Part);

    void Get_Hero_Bone_3D(Bone3D &h3D, Addr Mesh_Offset);

    void Get_Bone_2D(Bone2D &hero, float *matrix, int px, int py, Addr Mesh_Offset);

    static void D3DXMatrixMultiply(D3DXMATRIX &dxmatrix, D3DXMATRIX &Pm1, D3DXMATRIX &Pm2);

    static void getBone3Dto2D(C2D &d2D, const float *matrix, const int px, const int py, C3D &obj);

    void getBone3Dto2D(Bone3D &h3D, Bone2D &hero2D, float *matrix, int px, int py);

    void getPlayerName(Addr objAddr, char *buff, int *i);

    void getObjName(int id, Addr Gname,int len , char *buffer);


};



