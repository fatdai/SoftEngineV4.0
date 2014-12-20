//
//  Mesh.h
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#ifndef __SD_LIB__Mesh__
#define __SD_LIB__Mesh__

#include <stdio.h>
#include "FaceIndex.h"
#include "Camera.h"

enum{
    MESH_STATE_VISIBLE = 1,
    MESH_STATE_NEED_CUTDOWN = 1<<1, // 里面的三角形需要进行裁减
    MESH_STATE_CULLED = 1<<2, // 被cull掉
};

class Mesh{
    
public:
    
    Mesh();
    ~Mesh();
    
    // 状态
    int state;
    
    // 面数
    int num_faces;
    
    // 顶点个数
    int num_vertices;
    
    // 面索引数组
    FaceIndex* faceIndexs;
    
    // 顶点列表
    Vertex* vlist_local;
    Vertex* vlist_trans;
    
    // 材质
    Material mati;
    
    // 平均半径，进行碰撞监测
    float avg_radius;
    
    // 最大半径,进行 cull
    float max_radius;
    
public:
    
    // for test pipeline
    // 为了测试渲染管线,先就在里面进行处理
    
    // 修改状态
    void reset();
    
    // 分配内存
    void initMesh();
    
    // 变换到世界坐标系之前,先进行cull
    // 如果没有被剔除掉,则进行变换,并设置相关标志
    void modelToWorld(Camera* camera);
    
private:
    
    // 进行 cull
    bool cull(Camera* camera);
    
public:
    
    ///////////////////////////////////////////////////////
    //  在世界坐标系中的位置
    Point3 world_pos;
    
    // 饶  x , y ,z 轴旋转矩阵
    // 顺序为 xyz, 合适的时候再换成四元数
    // 暂时不考虑放大的情况
    Mat4 rotX;
    Mat4 rotY;
    Mat4 rotZ;

};

#endif /* defined(__SD_LIB__Mesh__) */
