//
//  RenderList.h
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#ifndef __SD_LIB__RenderList__
#define __SD_LIB__RenderList__

#include <stdio.h>
#include "Face.h"
#include "FaceIndex.h"
#include "Mesh.h"

class Camera;

const static int kNumOfMaxRenderFace = 20000;

//
//  渲染流程:
//     物体局部坐标---->对物体进行cull---->model_to_world----->插入到renderlist---->执行背面消除
//----> 进行相机变换 ----> 进行3D裁减 ----> 进行光照计算 ----> 进行透视变换 ----> 视口变换到屏幕
//

class RenderList{
    
public:
    
    enum{
        SORT_FACE_AVGZ,  // 平均z
        SORT_FACE_NEARZ, // 按照最小的z
        SORT_FACE_FARZ,  // 按照最大的z
    };
    
    RenderList();
    
    void reset();
    
    //  插入
    void insertFace(Face* face);
    void insertFaceIndex(FaceIndex* faceIndex);
    void insertMesh(Mesh* mesh);
    
    // 背面消除
    void removeBackface(Camera* camera);
    
    // 相机变换
    void worldToCamera(Camera* camera);
    
    // 3D裁减
    void curDown(Camera* camera);
    
    // 执行光照计算
    void light(Camera* camera);
    
    // 执行透视变换
    // 注意投影变换后z的变化
    // 比如 点x0(0,0,-5,1) 和点 x1(0,0,-10,1);
    // 投影变换前 x0 > x1
    // 投影变换后 x0 < x1
    void perspective(Camera* camera);
    
    // 到屏幕
    void toscreen(Camera* camera);
    
    // 进行z排序
    void sort(int sort_method = SORT_FACE_AVGZ);
    
    ////////////////////////////////////////////////////////
    // 绘制函数
    
    // 绘制线框
    // 被废弃
    void drawWire();
    
    // 绘制实体
    // 被废弃
    void drawSolid();
    
    // 绘制
    void draw();
    
public:
    
    // 需要渲染的面数
    int num_faces;
    
    // 所有需要渲染的面
    Face face_data[kNumOfMaxRenderFace];
    
private:
    
    static int compare_avgz_face(const void *arg1, const void *arg2);
    static int compare_farz_face(const void *arg1, const void *arg2);
    static int compare_nearz_face(const void *arg1, const void *arg2);
};

#endif /* defined(__SD_LIB__RenderList__) */
