//
//  Camera.h
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __Soft_Engine_V3_0__Camera__
#define __Soft_Engine_V3_0__Camera__

#include <iostream>
#include "ZCMath.h"


enum class CameraMode{
    CAM_MODEL_UVN_SIMPLE,
    CAM_MODEL_UVN_SPHERICAL,
    CAM_MODEL_EULER,
};

// 欧拉相机顺序
enum class EulerCameraRotSeq{
    CAM_ROT_SEQ_XYZ,
    CAM_ROT_SEQ_YXZ,
    CAM_ROT_SEQ_XZY,
    CAM_ROT_SEQ_YZX,
    CAM_ROT_SEQ_ZYX,
    CAM_ROT_SEQ_ZXY
};

enum{
    CAM_DIR_DIRTY = 1,
    CAM_POS_DIRTY = 1<<1,
};

class Camera{
    
private:
    
    // 计算投影矩阵 视口矩阵 和  裁减平面和
    void buidProjScreenMatrixAndClipPlane();

public:
    
    // 欧拉角相机
    Camera(const Point3& cam_pos, // 相机位置
           const Vec3& cam_dir, // 相机的方位
           float cam_near,
           float cam_far,
           float cam_fovy,
           float screen_w,
           float screen_h,
           EulerCameraRotSeq seq);
    
    // UVN相机
    Camera(const Point3& cam_pos,
           const Vec3& var, // simple_uvn 模式下表示 观察点, 球面 uvn 模式下表示相机的方位
           float cam_near,
           float cam_far,
           float cam_fovy,
           float screen_w,
           float screen_h,
           CameraMode uvnMode
           );
    
    void setPosition(const Point3& pPos);
    void setDir(const Vec3& pDir);
    
    void update();
    
public:
    
    int flags;
    
    int state;
    int attr;
    
    Point3 pos;
    
    // 表示相机方位
    // x 表示仰角
    // y 表示方位角
    Vec3 dir;
    
    //uvn
    Vec3 u,v,n;
    
    // 视角
    float fovy;
    
    float near;
    float far;
    
    float viewport_w,viewport_h;
    
    float aspect_ratio;
    
    // 裁减面
    Plane3D near_clip_plane;
    Plane3D far_clip_plane;
    Plane3D left_clip_plane;
    Plane3D right_clip_plane;
    Plane3D top_clip_plane;
    Plane3D bottom_clip_plane;
    
    // 相机矩阵
    Mat4 mcam;
    
    // 投影矩阵
    Mat4 mper;
    
    // 视口矩阵
    Mat4 mscr;
    
    // 相机模型
    CameraMode cameraMode;
    EulerCameraRotSeq eulerRotSeq;
    
};

#endif /* defined(__Soft_Engine_V3_0__Camera__) */
