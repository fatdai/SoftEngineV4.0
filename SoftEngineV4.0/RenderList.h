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
//     物体局部坐标---->model_to_world---->对物体进行cull----->插入到renderlist---->执行背面消除
//----> 进行相机变换 ----> 进行3D裁减 ----> 进行光照计算 ----> 进行透视变换 ----> 视口变换到屏幕
//

class RenderList{
    
public:
    
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
    void perspective(Camera* camera);
    
    // 到屏幕
    void toscreen(Camera* camera);
    
    ////////////////////////////////////////////////////////
    // 绘制函数
    
    // 绘制线框
    void drawWire();
    
    // 绘制实体
    void drawSolid();
    
public:
    
    // 需要渲染的面数
    int num_faces;
    
    // 所有需要渲染的面
    Face face_data[kNumOfMaxRenderFace];
    
};

#endif /* defined(__SD_LIB__RenderList__) */
