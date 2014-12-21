//
//  Mesh.cpp
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#include "Mesh.h"
#include "Face.h"
#include <assert.h>

Mesh::Mesh(){
    state = 0;
    num_faces = num_vertices = 0;
    vlist_local = nullptr;
    vlist_trans = nullptr;
    faceIndexs = nullptr;
    
    rotX = Mat4::IDENTITY;
    rotY = Mat4::IDENTITY;
    rotZ = Mat4::IDENTITY;
}

Mesh::~Mesh(){
    if (vlist_local) {
        delete [] vlist_local;
        vlist_local = nullptr;
    }
    
    if (vlist_trans) {
        delete [] vlist_trans;
        vlist_trans = nullptr;
    }
    
    if (faceIndexs) {
        delete [] faceIndexs;
        faceIndexs = nullptr;
    }
}

void Mesh::reset(){
    
    RESET_BIT(state, MESH_STATE_CULLED);
    RESET_BIT(state, MESH_STATE_NEED_CUTDOWN);
    
    // 重置多边形被裁减和背面标志
    for (int i = 0; i < num_faces; ++i) {
        FaceIndex* cur_face_index = &faceIndexs[i];
        
        if (!(cur_face_index->state & FACE_STATE_ACTIVITY)) {
            continue;
        }
        
        // 可以进行优化下
        RESET_BIT(cur_face_index->state, FACE_STATE_CLIPPED);
        RESET_BIT(cur_face_index->state, FACE_STATE_NEED_CUTDOWN);
        RESET_BIT(cur_face_index->state, FACE_STATE_CLIPPED);
    }
}

void Mesh::initMesh(){
    assert(num_vertices > 0 && num_faces > 0);
    vlist_local = new Vertex[num_vertices];
    vlist_trans = new Vertex[num_vertices];
    faceIndexs = new FaceIndex[num_faces];
}

// 变换到世界坐标系
void Mesh::modelToWorld(Camera* camera){
    if (!cull(camera)) {
    
        Mat4 mat = rotZ * rotY * rotX;
        Vec3 tmpPoint;
        if (state & MESH_STATE_NEED_CUTDOWN) {
            for (int i = 0; i < num_faces; ++i) {
                SET_BIT(faceIndexs[i].state,FACE_STATE_NEED_CUTDOWN);
            }
        }
        
        for (int i = 0;i < num_vertices; ++i) {
            // 变换顶点
            if (vlist_local[i].attr & VERTEX_ATTR_POINT) {
                mat.transformPoint(vlist_local[i].v,&tmpPoint);
                vlist_trans[i].v = world_pos + tmpPoint;
            }
            
            // 变换法线
            if (vlist_local[i].attr & VERTEX_ATTR_NORMAL) {
                mat.transformVector(vlist_local[i].n,&vlist_trans[i].n);
                vlist_trans[i].t = vlist_local[i].t;
            }
        }
    }
}

// 进行 cull
bool Mesh::cull(Camera* camera){
    
    // 世界在世界坐标系下进行cull判断
    float distance = camera->bottom_clip_plane.distance(world_pos);
    if (distance <= -max_radius) {
        state |= MESH_STATE_CULLED;
        return true;
    }else if (fabsf(distance) < max_radius){
        // 相交
        state |= MESH_STATE_NEED_CUTDOWN;
    }
    
    //top_clip_plane
    distance = camera->top_clip_plane.distance(world_pos);
    if (distance <= -max_radius) {
        state |= MESH_STATE_CULLED;
        return true;
    }else if (fabsf(distance) < max_radius){
        // 相交
        state |= MESH_STATE_NEED_CUTDOWN;
    }
    
    //left_clip_plane
    distance = camera->left_clip_plane.distance(world_pos);
    if (distance <= -max_radius) {
        state |= MESH_STATE_CULLED;
        return true;
    }else if (fabsf(distance) < max_radius){
        // 相交
        state |= MESH_STATE_NEED_CUTDOWN;
    }
    
    //right_clip_plane
    distance = camera->right_clip_plane.distance(world_pos);
    if (distance <= -max_radius) {
        state |= MESH_STATE_CULLED;
        return true;
    }else if (fabsf(distance) < max_radius){
        // 相交
        state |= MESH_STATE_NEED_CUTDOWN;
    }
    
    //near_clip_plane
    distance = camera->near_clip_plane.distance(world_pos);
    if (distance <= -max_radius) {
        state |= MESH_STATE_CULLED;
        return true;
    }else if (fabsf(distance) < max_radius){
        // 相交
        state |= MESH_STATE_NEED_CUTDOWN;
    }
    
    //far_clip_plane
    distance = camera->far_clip_plane.distance(world_pos);
    if (distance <= -max_radius) {
        state |= MESH_STATE_CULLED;
        return true;
    }else if (fabsf(distance) < max_radius){
        // 相交
        state |= MESH_STATE_NEED_CUTDOWN;
    }
    
    return false;
}


void Mesh::setRotateX(float degressX){
    Mat4::createRotationX(MATH_DEG_TO_RAD(degressX),&rotX);
}
void Mesh::setRotateY(float degressY){
    Mat4::createRotationY(MATH_DEG_TO_RAD(degressY),&rotY);
}
void Mesh::setRotateZ(float degressZ){
    Mat4::createRotationZ(MATH_DEG_TO_RAD(degressZ),&rotZ);
}

void Mesh::setColor(const Color& color){
    for (int i = 0;i < num_faces; ++i) {
        faceIndexs[i].lit_color[0] = color;
    }
}
