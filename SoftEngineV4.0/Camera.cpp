//
//  Camera.cpp
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "Camera.h"


Camera::Camera(const Point3& cam_pos, // 相机位置
               const Vec3& cam_dir, // 相机的方位
               float cam_near,
               float cam_far,
               float cam_fovy,
               float screen_w,
               float screen_h,
               EulerCameraRotSeq seq){
    
    flags = 0;
    cameraMode = CameraMode::CAM_MODEL_EULER;
    eulerRotSeq = seq;
    
    pos = cam_pos;
    dir = cam_dir;
    near = cam_near;
    far = cam_far;
    fovy = cam_fovy;
    viewport_w = screen_w;
    viewport_h = screen_h;
    
    aspect_ratio = viewport_w/viewport_h;
    
    // 先提取欧拉角的三个角度
    float angle_x = MATH_DEG_TO_RAD(dir.x);
    float angle_y = MATH_DEG_TO_RAD(dir.y);
    float angle_z = MATH_DEG_TO_RAD(dir.z);
    
    // 构造绕x的旋转矩阵
    Mat4 rotateX;
    Mat4::createRotationX(angle_x,&rotateX);
    rotateX.transpose();
    
    Mat4 rotateY;
    Mat4::createRotationY(angle_y,&rotateY);
    rotateY.transpose();
    
    Mat4 rotateZ;
    Mat4::createRotationZ(angle_z,&rotateZ);
    rotateZ.transpose();
    
    Mat4 inv_trans;
    Mat4::createTranslation(-pos,&inv_trans);
    
    Mat4 temp1,temp2;
    
    // 根据定义的顺序来计算
    switch (seq) {
        case EulerCameraRotSeq::CAM_ROT_SEQ_XYZ:
            Mat4::multiply(rotateX,rotateY,&temp1);
            Mat4::multiply(temp1,rotateZ,&temp2);
            Mat4::multiply(temp2,inv_trans,&mcam);
            break;
        case EulerCameraRotSeq::CAM_ROT_SEQ_XZY:
            Mat4::multiply(rotateX,rotateZ,&temp1);
            Mat4::multiply(temp1,rotateY,&temp2);
            Mat4::multiply(temp2,inv_trans,&mcam);
            break;
        case EulerCameraRotSeq::CAM_ROT_SEQ_YXZ:
            Mat4::multiply(rotateY,rotateX,&temp1);
            Mat4::multiply(temp1,rotateZ,&temp2);
            Mat4::multiply(temp2,inv_trans,&mcam);
            break;
        case EulerCameraRotSeq::CAM_ROT_SEQ_YZX:
            Mat4::multiply(rotateY,rotateZ,&temp1);
            Mat4::multiply(temp1,rotateX,&temp2);
            Mat4::multiply(temp2,inv_trans,&mcam);
            break;
        case EulerCameraRotSeq::CAM_ROT_SEQ_ZXY:
            Mat4::multiply(rotateZ,rotateX,&temp1);
            Mat4::multiply(temp1,rotateY,&temp2);
            Mat4::multiply(temp2,inv_trans,&mcam);
            break;
        case EulerCameraRotSeq::CAM_ROT_SEQ_ZYX:
            Mat4::multiply(rotateZ,rotateY,&temp1);
            Mat4::multiply(temp1,rotateX,&temp2);
            Mat4::multiply(temp2,inv_trans,&mcam);
            break;
        default:
            printf("unsupperted rot seq.....\n");
            break;
    }
    
    // uvn
    temp2.transpose();
    temp2.getColumnX(&u);
    temp2.getColumnY(&v);
    temp2.getColumnZ(&n);
    
    // 计算裁减平面和相关矩阵
    buidProjScreenMatrixAndClipPlane();
}


Camera::Camera(const Point3& cam_pos,
       const Vec3& var, // simple_uvn 模式下表示 观察点, 球面 uvn 模式下表示相机的方位
       float cam_near,
       float cam_far,
       float cam_fovy,
       float screen_w,
       float screen_h,
       CameraMode uvnMode
       ){
 
    flags = 0;
    cameraMode = uvnMode;
    pos = cam_pos;
    near = cam_near;
    far = cam_far;
    fovy = cam_fovy;
    viewport_w = screen_w;
    viewport_h = screen_h;
    
    aspect_ratio = viewport_w/viewport_h;
    
    if (uvnMode == CameraMode::CAM_MODEL_UVN_SIMPLE) {
        // var 表示的是 target
        Vec3 dir3 = var - pos;
        dir3.normalize();
        
        // 计算 uvn
        n = -dir3;
        Vec3::cross(dir3,Vec3::UNIT_Y,&u);
        u.normalize();
        
        Vec3::cross(u,dir3,&v);
        v.normalize();
    }else if (uvnMode == CameraMode::CAM_MODEL_UVN_SPHERICAL){
        // var 表示的是方位
        // 这里假设不围绕z轴旋转
        dir = var;
        
        // 仰角
        float fi = dir.x;
        
        // 方位角
        float st = dir.y;
        
        float sin_fi = sinf(MATH_DEG_TO_RAD(fi));
        float cos_fi = cosf(MATH_DEG_TO_RAD(fi));
        
        float sin_st = sinf(MATH_DEG_TO_RAD(st));
        float cos_st = cosf(MATH_DEG_TO_RAD(st));
        
        // 计算uvn
        float x = -cos_fi * sin_st;
        float y = sin_fi;
        float z = -cos_fi * cos_st;
        
        Vec3 dir3(x,y,z);
        n = -dir3;
        Vec3::cross(dir3,Vec3::UNIT_Y,&u);
        u.normalize();
        
        Vec3::cross(u,dir3,&v);
        v.normalize();

    }else{
        printf("unsupport uvn mode.....\n");
    }
    
    float inv_uvn_arr[16]={
        u.x,v.x,n.x,0,
        u.y,v.y,n.y,0,
        u.z,v.z,n.z,0,
        0,0,0,1
    };
    Mat4 inv_uvn(inv_uvn_arr);
    
    Mat4 inv_trans;
    Mat4::createTranslation(-pos, &inv_trans);
    
    Mat4::multiply(inv_uvn,inv_trans,&mcam);
    
    buidProjScreenMatrixAndClipPlane();
}


void Camera::buidProjScreenMatrixAndClipPlane(){
    
    // 投影矩阵一般没变化
    Mat4::createPerspective(fovy,aspect_ratio,near,far,&mper);
    
    // 视口矩阵一般也没变化
    float scr[16] = {
        (viewport_w-1)/2.f,0,0,0,
        0,-(viewport_h-1)/2.f,0,0,
        0,0,1,0,
        viewport_w/2.f,viewport_h/2.f,0,1
    };
    mscr.set(scr);
    
    //计算裁减平面
    Vec3 dir3 = -n;
    
    // near
    near_clip_plane.p0 = pos + near * dir3;
    near_clip_plane.n = dir3;
    
    // far
    far_clip_plane.p0 = pos + far * dir3;
    far_clip_plane.n = n;
    
    float rad = MATH_DEG_TO_RAD(fovy/2.0);
    float half_h = tanf(rad) * near;
    float half_w = aspect_ratio * half_h;
    
    // near left bottom
    Point3 left_bottom = near_clip_plane.p0 - half_h * v - half_w * u;
    Point3 left_top = left_bottom + 2*v*half_h;
    Point3 right_bottom = left_bottom + 2 * u * half_w;
    Point3 right_top = left_top + 2 * u * half_w;
    Vec3 tmp_v1 = left_bottom - pos;
    Vec3 tmp_v2 = left_top - pos;
 
    // left,画图计算
    left_clip_plane.p0 = pos;
    Vec3::cross(tmp_v1,tmp_v2,&left_clip_plane.n);
    left_clip_plane.n.normalize();
    
    // right
    right_clip_plane.p0 = pos;
    Vec3 tmp_v3 = right_top - pos;
    Vec3 tmp_v4 = right_bottom - pos;
    Vec3::cross(tmp_v3,tmp_v4,&right_clip_plane.n);
    right_clip_plane.n.normalize();
    
    //top
    top_clip_plane.p0 = pos;
    Vec3::cross(tmp_v2,tmp_v3,&top_clip_plane.n);
    top_clip_plane.n.normalize();
    
    //bottom
    bottom_clip_plane.p0 = pos;
    Vec3::cross(tmp_v4,tmp_v1,&bottom_clip_plane.n);
    bottom_clip_plane.n.normalize();
}

void Camera::setPosition(const Point3& pPos){
    pos = pPos;
    flags |= CAM_POS_DIRTY;
}

void Camera::setDir(const Vec3& pDir){
    dir = pDir;
    flags |= CAM_DIR_DIRTY;
}

void Camera::update(){
    
    if (flags & CAM_POS_DIRTY ||
        flags & CAM_DIR_DIRTY) {
        
        if (cameraMode == CameraMode::CAM_MODEL_UVN_SIMPLE) {
            // var 表示的是 target
            Vec3 dir3 = dir - pos;
            dir3.normalize();
            
            // 计算 uvn
            n = -dir3;
            Vec3::cross(dir3,Vec3::UNIT_Y,&u);
            u.normalize();
            
            Vec3::cross(u,dir3,&v);
            v.normalize();
            
            
            float inv_uvn_arr[16]={
                u.x,v.x,n.x,0,
                u.y,v.y,n.y,0,
                u.z,v.z,n.z,0,
                0,0,0,1
            };
            Mat4 inv_uvn(inv_uvn_arr);
            
            Mat4 inv_trans;
            Mat4::createTranslation(-pos, &inv_trans);
            
            Mat4::multiply(inv_uvn,inv_trans,&mcam);
            
        }else if (cameraMode == CameraMode::CAM_MODEL_UVN_SPHERICAL){
            // var 表示的是方位
            // 这里假设不围绕z轴旋转
            
            // 仰角
            float fi = dir.x;
            
            // 方位角
            float st = dir.y;
            
            float sin_fi = sinf(MATH_DEG_TO_RAD(fi));
            float cos_fi = cosf(MATH_DEG_TO_RAD(fi));
            
            float sin_st = sinf(MATH_DEG_TO_RAD(st));
            float cos_st = cosf(MATH_DEG_TO_RAD(st));
            
            // 计算uvn
            float x = -cos_fi * sin_st;
            float y = sin_fi;
            float z = -cos_fi * cos_st;
            
            Vec3 dir3(x,y,z);
            n = -dir3;
            Vec3::cross(dir3,Vec3::UNIT_Y,&u);
            u.normalize();
            
            Vec3::cross(u,dir3,&v);
            v.normalize();
            
            float inv_uvn_arr[16]={
                u.x,v.x,n.x,0,
                u.y,v.y,n.y,0,
                u.z,v.z,n.z,0,
                0,0,0,1
            };
            Mat4 inv_uvn(inv_uvn_arr);
            
            Mat4 inv_trans;
            Mat4::createTranslation(-pos, &inv_trans);
            
            Mat4::multiply(inv_uvn,inv_trans,&mcam);
            
        }else if (cameraMode == CameraMode::CAM_MODEL_EULER){
            
            // 先提取欧拉角的三个角度
            float angle_x = MATH_DEG_TO_RAD(dir.x);
            float angle_y = MATH_DEG_TO_RAD(dir.y);
            float angle_z = MATH_DEG_TO_RAD(dir.z);
            
            // 构造绕x的旋转矩阵
            Mat4 rotateX;
            Mat4::createRotationX(angle_x,&rotateX);
            rotateX.transpose();
            
            Mat4 rotateY;
            Mat4::createRotationY(angle_y,&rotateY);
            rotateY.transpose();
            
            Mat4 rotateZ;
            Mat4::createRotationZ(angle_z,&rotateZ);
            rotateZ.transpose();
            
            Mat4 inv_trans;
            Mat4::createTranslation(-pos,&inv_trans);
            
            Mat4 temp1,temp2;
            
            // 根据定义的顺序来计算
            switch (eulerRotSeq) {
                case EulerCameraRotSeq::CAM_ROT_SEQ_XYZ:
                    Mat4::multiply(rotateX,rotateY,&temp1);
                    Mat4::multiply(temp1,rotateZ,&temp2);
                    Mat4::multiply(temp2,inv_trans,&mcam);
                    break;
                case EulerCameraRotSeq::CAM_ROT_SEQ_XZY:
                    Mat4::multiply(rotateX,rotateZ,&temp1);
                    Mat4::multiply(temp1,rotateY,&temp2);
                    Mat4::multiply(temp2,inv_trans,&mcam);
                    break;
                case EulerCameraRotSeq::CAM_ROT_SEQ_YXZ:
                    Mat4::multiply(rotateY,rotateX,&temp1);
                    Mat4::multiply(temp1,rotateZ,&temp2);
                    Mat4::multiply(temp2,inv_trans,&mcam);
                    break;
                case EulerCameraRotSeq::CAM_ROT_SEQ_YZX:
                    Mat4::multiply(rotateY,rotateZ,&temp1);
                    Mat4::multiply(temp1,rotateX,&temp2);
                    Mat4::multiply(temp2,inv_trans,&mcam);
                    break;
                case EulerCameraRotSeq::CAM_ROT_SEQ_ZXY:
                    Mat4::multiply(rotateZ,rotateX,&temp1);
                    Mat4::multiply(temp1,rotateY,&temp2);
                    Mat4::multiply(temp2,inv_trans,&mcam);
                    break;
                case EulerCameraRotSeq::CAM_ROT_SEQ_ZYX:
                    Mat4::multiply(rotateZ,rotateY,&temp1);
                    Mat4::multiply(temp1,rotateX,&temp2);
                    Mat4::multiply(temp2,inv_trans,&mcam);
                    break;
                default:
                    printf("unsupperted rot seq.....\n");
                    break;
            }
            
            // uvn
            temp2.transpose();
            temp2.getColumnX(&u);
            temp2.getColumnY(&v);
            temp2.getColumnZ(&n);
        }
        
        buidProjScreenMatrixAndClipPlane();
        
        flags = 0;
    }
}

