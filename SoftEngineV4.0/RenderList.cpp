//
//  RenderList.cpp
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#include "RenderList.h"
#include "Camera.h"
#include "Material.h"
#include "Color.h"
#include "Light.h"
#include "Render_1.h"
#include "Render_2.h"

RenderList::RenderList(){
    num_faces = 0;
}

void RenderList::reset(){
    num_faces = 0;
}

void RenderList::insertFace(Face* face){
    
    if (num_faces >= kNumOfMaxRenderFace) {
        printf("达到最大处理的面数.\n");
        return;
    }
    
    face_data[num_faces] = *face;
    ++num_faces;
}

void RenderList::insertFaceIndex(FaceIndex* faceIndex){
    
    if (num_faces >= kNumOfMaxRenderFace) {
        printf("达到最大处理的面数.\n");
        return;
    }
    
    face_data[num_faces].state = faceIndex->state;
    face_data[num_faces].mati = faceIndex->mati;
    
    face_data[num_faces].lit_color[0] = faceIndex->lit_color[0];
    face_data[num_faces].lit_color[1] = faceIndex->lit_color[1];
    face_data[num_faces].lit_color[2] = faceIndex->lit_color[2];
    
    face_data[num_faces].vlist_trans[0] = faceIndex->vlist[faceIndex->vert[0]];
    face_data[num_faces].vlist_trans[1] = faceIndex->vlist[faceIndex->vert[1]];
    face_data[num_faces].vlist_trans[2] = faceIndex->vlist[faceIndex->vert[2]];
    
    ++num_faces;
}

void RenderList::insertMesh(Mesh* mesh){
    if (!(mesh->state & MESH_STATE_VISIBLE) ||
        (mesh->state & MESH_STATE_CULLED)) {
        return;
    }
    
    for (int i = 0; i < mesh->num_faces; ++i) {
        FaceIndex* face_index = &(mesh->faceIndexs[i]);
        
        // 指向变换后的顶点
        Vertex* old_local = face_index->vlist;
        Material old_mati = face_index->mati;
        
        face_index->vlist = mesh->vlist_trans;
        face_index->mati = mesh->mati;
        
        insertFaceIndex(face_index);
        
        face_index->vlist = old_local;
        face_index->mati = old_mati;
    }
}

void RenderList::removeBackface(Camera* camera){
    
    for (int i = 0; i < num_faces; ++i) {
        Face* face = &(face_data[i]);
        
        Vertex* v0 = &face->vlist_trans[0];
        Vertex* v1 = &face->vlist_trans[1];
        Vertex* v2 = &face->vlist_trans[2];
        
        Vec3 v01 = v1->v - v0->v;
        Vec3 v02 = v2->v - v0->v;
        Vec3 vn;
        Vec3::cross(v01,v02,&vn);
        
        // 计算观察方向
        Vec3 view =  v0->v - camera->pos;
        
        if (view.dot(vn) >= 0) {
            face->state |= FACE_STATE_BACKFACE;
        }
    }
}

void RenderList::worldToCamera(Camera* camera){
    
    for (int i = 0; i < num_faces; ++i) {
        Face* cur_face = &face_data[i];
        
        if (!(cur_face->state & FACE_STATE_ACTIVITY) ||
            cur_face->state & FACE_STATE_BACKFACE) {
            continue;
        }
        
        for (int vertex = 0; vertex < 3; ++vertex) {
            camera->mcam.transformPoint(cur_face->vlist_trans[vertex].v,&cur_face->vlist_trans[vertex].v);
        }
    }
}

// 进行3d裁减
void RenderList::curDown(Camera* camera){
    
    // internal clipping codes
#define CLIP_CODE_GZ   0x0001    // z > z_max
#define CLIP_CODE_LZ   0x0002    // z < z_min
#define CLIP_CODE_IZ   0x0004    // z_min < z < z_max
    
#define CLIP_CODE_GX   0x0001    // x > x_max
#define CLIP_CODE_LX   0x0002    // x < x_min
#define CLIP_CODE_IX   0x0004    // x_min < x < x_max
    
#define CLIP_CODE_GY   0x0001    // y > y_max
#define CLIP_CODE_LY   0x0002    // y < y_min
#define CLIP_CODE_IY   0x0004    // y_min < y < y_max
    
#define CLIP_CODE_NULL 0x0000
    
    int vertex_ccodes[3]; // used to store clipping flags
    
    int num_verts_in;     // number of vertices inside
    
    int v0, v1, v2;       // vertex indices
    
    // 避免重复裁减
    int face_count = num_faces;
    
    for (int i = 0; i < face_count; ++i) {
        
        Face* cur_face = &face_data[i];
        
        if (!(cur_face->state & FACE_STATE_ACTIVITY) ||
            (cur_face->state & FACE_STATE_BACKFACE)){
            continue;
        }
        
        // 这里最对需要进行判断和裁减的进行处理
        // 完全在视锥体里面的都不需要进行判断
        if (cur_face->state & FACE_STATE_NEED_CUTDOWN) {
            
            float z_factor = 0.5*camera->viewport_w/camera->near;
            
            // 画图理解
            // vertex0
            float z_test = -z_factor * cur_face->vlist_trans[0].z;
            
            if (cur_face->vlist_trans[0].x > z_test) {
                vertex_ccodes[0] = CLIP_CODE_GX;
            }else if (cur_face->vlist_trans[0].x < -z_test){
                vertex_ccodes[0] = CLIP_CODE_LX;
            }else{
                vertex_ccodes[0] = CLIP_CODE_IX;
            }
            
            //vertex1
            z_test = -z_factor * cur_face->vlist_trans[1].z;
            
            if (cur_face->vlist_trans[1].x > z_test) {
                vertex_ccodes[1] = CLIP_CODE_GX;
            }else if (cur_face->vlist_trans[1].x < -z_test){
                vertex_ccodes[1] = CLIP_CODE_LX;
            }else{
                vertex_ccodes[1] = CLIP_CODE_IX;
            }
            
            //vertex2
            z_test = -z_factor * cur_face->vlist_trans[2].z;
            
            if (cur_face->vlist_trans[2].x > z_test) {
                vertex_ccodes[2] = CLIP_CODE_GX;
            }else if (cur_face->vlist_trans[2].x < -z_test){
                vertex_ccodes[2] = CLIP_CODE_LX;
            }else{
                vertex_ccodes[2] = CLIP_CODE_IX;
            }
            
            
            // clipping planes
            if ( ((vertex_ccodes[0] == CLIP_CODE_GX) &&
                  (vertex_ccodes[1] == CLIP_CODE_GX) &&
                  (vertex_ccodes[2] == CLIP_CODE_GX) ) ||
                
                ((vertex_ccodes[0] == CLIP_CODE_LX) &&
                 (vertex_ccodes[1] == CLIP_CODE_LX) &&
                 (vertex_ccodes[2] == CLIP_CODE_LX) ) )
                
            {
                // clip the poly completely out of frustrum
                cur_face->state |= FACE_STATE_CLIPPED;
                
                // move on to next polygon
                continue;
            } // end if
            
            
            ///////////////////////////////////////////
            //  y方向 进行裁减
            z_factor = 0.5*camera->viewport_h/camera->near;
            
            // vertex0
            z_test = -z_factor * cur_face->vlist_trans[0].z;
            
            if (cur_face->vlist_trans[0].y > z_test) {
                vertex_ccodes[0] = CLIP_CODE_GY;
            }else if (cur_face->vlist_trans[0].y < -z_test){
                vertex_ccodes[0] = CLIP_CODE_LY;
            }else{
                vertex_ccodes[0] = CLIP_CODE_IY;
            }
            
            //vertex1
            z_test = -z_factor * cur_face->vlist_trans[1].z;
            
            if (cur_face->vlist_trans[1].y > z_test) {
                vertex_ccodes[1] = CLIP_CODE_GY;
            }else if (cur_face->vlist_trans[1].y < -z_test){
                vertex_ccodes[1] = CLIP_CODE_LY;
            }else{
                vertex_ccodes[1] = CLIP_CODE_IY;
            }
            
            //vertex2
            z_test = -z_factor * cur_face->vlist_trans[2].z;
            
            if (cur_face->vlist_trans[2].y > z_test) {
                vertex_ccodes[2] = CLIP_CODE_GY;
            }else if (cur_face->vlist_trans[2].y < -z_test){
                vertex_ccodes[2] = CLIP_CODE_LY;
            }else{
                vertex_ccodes[2] = CLIP_CODE_IY;
            }
            
            // clipping planes
            if ( ((vertex_ccodes[0] == CLIP_CODE_GY) &&
                  (vertex_ccodes[1] == CLIP_CODE_GY) &&
                  (vertex_ccodes[2] == CLIP_CODE_GY) ) ||
                
                ((vertex_ccodes[0] == CLIP_CODE_LY) &&
                 (vertex_ccodes[1] == CLIP_CODE_LY) &&
                 (vertex_ccodes[2] == CLIP_CODE_LY) ) )
                
            {
                // clip the poly completely out of frustrum
                cur_face->state |= FACE_STATE_CLIPPED;
                
                // move on to next polygon
                continue;
            } // end if
            
            ///////////////////////////////////////////
            /// z 方向进行裁减,可能还需要进行分割
            num_verts_in = 0;
            
            // vertex0
            if (cur_face->vlist_trans[0].z < -camera->far) {
                vertex_ccodes[0] = CLIP_CODE_GZ;
            }else if (cur_face->vlist_trans[0].z > -camera->near){
                vertex_ccodes[0] = CLIP_CODE_LZ;
            }else{
                vertex_ccodes[0] = CLIP_CODE_IZ;
                ++num_verts_in;
            }
            
            //vertex1
            if (cur_face->vlist_trans[1].z < -camera->far) {
                vertex_ccodes[1] = CLIP_CODE_GZ;
            }else if (cur_face->vlist_trans[1].z > -camera->near){
                vertex_ccodes[1] = CLIP_CODE_LZ;
            }else{
                vertex_ccodes[1] = CLIP_CODE_IZ;
                ++num_verts_in;
            }
            
            //vertex2
            if (cur_face->vlist_trans[2].z < -camera->far) {
                vertex_ccodes[2] = CLIP_CODE_GZ;
            }else if (cur_face->vlist_trans[2].z > -camera->near){
                vertex_ccodes[2] = CLIP_CODE_LZ;
            }else{
                vertex_ccodes[2] = CLIP_CODE_IZ;
                ++num_verts_in;
            }
            
            // test for trivial rejections, polygon completely beyond far or near
            // z clipping planes
            if ( ((vertex_ccodes[0] == CLIP_CODE_GZ) &&
                  (vertex_ccodes[1] == CLIP_CODE_GZ) &&
                  (vertex_ccodes[2] == CLIP_CODE_GZ) ) ||
                
                ((vertex_ccodes[0] == CLIP_CODE_LZ) &&
                 (vertex_ccodes[1] == CLIP_CODE_LZ) &&
                 (vertex_ccodes[2] == CLIP_CODE_LZ) ) )
                
            {
                // clip the poly completely out of frustrum
                cur_face->state |= FACE_STATE_CLIPPED;
                
                // move on to next polygon
                continue;
            } // end if
            
            
            ///////////////////////////////////////////////////////////
            /// 与near平面相交需要进行分割
            /// 与far平面相交则不需要进行处理
            // test if any vertex has protruded beyond near clipping plane?
            if ( ( (vertex_ccodes[0] | vertex_ccodes[1] | vertex_ccodes[2]) & CLIP_CODE_LZ) ){
                
                // 只有一个顶点在视锥体里面
                if (num_verts_in == 1) {
                    
                    // 寻找顺序
                    if (vertex_ccodes[0] == CLIP_CODE_IZ) {
                        v0 = 0;
                        v1 = 1;
                        v2 = 2;
                    }else if (vertex_ccodes[1] == CLIP_CODE_IZ){
                        v0 = 1;
                        v1 = 2;
                        v2 = 0;
                    }else{
                        v0 = 2;
                        v1 = 0;
                        v2 = 1;
                    }
                    
                    // 计算交点,用直线参数方程计算
                    // v0->v1
                    Vec3 vn = cur_face->vlist_trans[v1].v - cur_face->vlist_trans[v0].v;
                    vn.normalize();
                    
                    float t1 = (-camera->near - cur_face->vlist_trans[v0].z)/vn.z;
                    
                    // 计算 x y
                    float xi = cur_face->vlist_trans[v0].x + vn.x * t1;
                    float yi = cur_face->vlist_trans[v0].y + vn.y * t1;
                    
                    // 直接替换之前的顶点
                    cur_face->vlist_trans[v1].x = xi;
                    cur_face->vlist_trans[v1].y = yi;
                    cur_face->vlist_trans[v1].z = -camera->near;
                    
                    // v0->v2
                    vn = cur_face->vlist_trans[v2].v - cur_face->vlist_trans[v0].v;
                    vn.normalize();
                    
                    float t2 = (-camera->near - cur_face->vlist_trans[v0].z)/vn.z;
                    
                    //计算 x y
                    xi = cur_face->vlist_trans[v0].x + vn.x * t2;
                    yi = cur_face->vlist_trans[v0].y + vn.y * t2;
                    
                    // 直接替换之前的顶点
                    cur_face->vlist_trans[v2].x = xi;
                    cur_face->vlist_trans[v2].y = yi;
                    cur_face->vlist_trans[v2].z = -camera->near;
                    
                    // 如果有纹理,则还需要进行裁减纹理坐标
                    if (cur_face->mati.mati_type & Material::TEXTURE) {
                        float ui = cur_face->vlist_trans[v0].tu + (cur_face->vlist_trans[v1].tu - cur_face->vlist_trans[v0].tu) * t1;
                        float vi = cur_face->vlist_trans[v0].tv + (cur_face->vlist_trans[v1].tv - cur_face->vlist_trans[v0].tv) * t1;
                        cur_face->vlist_trans[v1].tu = ui;
                        cur_face->vlist_trans[v1].tv = vi;
                        
                        ui = cur_face->vlist_trans[v0].tu + (cur_face->vlist_trans[v2].tu - cur_face->vlist_trans[v0].tu) * t2;
                        vi = cur_face->vlist_trans[v0].tv + (cur_face->vlist_trans[v2].tv - cur_face->vlist_trans[v0].tv) * t2;
                        cur_face->vlist_trans[v2].tu = ui;
                        cur_face->vlist_trans[v2].tv = vi;
                    }
                } // end of  num_verts_in == 1
                else if (num_verts_in == 2){
                    
                    if (vertex_ccodes[0] == CLIP_CODE_LZ) {
                        v0 = 0;
                        v1 = 1;
                        v2 = 2;
                    }else if (vertex_ccodes[1] == CLIP_CODE_LZ){
                        v0 = 1;
                        v1 = 2;
                        v2 = 0;
                    }else{
                        v0 = 2;
                        v1 = 0;
                        v2 = 1;
                    }
                    
                    Face temp_poly = *(cur_face);
                    
                    // 将分割成2个三角形
                    // v0->v1
                    Vec3 vn = cur_face->vlist_trans[v1].v - cur_face->vlist_trans[v0].v;
                    vn.normalize();
                    
                    float t1 = (-camera->near - cur_face->vlist_trans[v0].z)/vn.z;
                    
                    // 计算 x y
                    float x01i = cur_face->vlist_trans[v0].x + vn.x * t1;
                    float y01i = cur_face->vlist_trans[v0].y + vn.y * t1;
                    
                    // v0->v2
                    vn = cur_face->vlist_trans[v2].v - cur_face->vlist_trans[v0].v;
                    vn.normalize();
                    
                    float t2 = (-camera->near - cur_face->vlist_trans[v0].z)/vn.z;
                    
                    //计算 x y
                    float x02i = cur_face->vlist_trans[v0].x + vn.x * t2;
                    float y02i = cur_face->vlist_trans[v0].y + vn.y * t2;
                    
                    // now overwrite vertex with new vertex
                    cur_face->vlist_trans[v0].x = x01i;
                    cur_face->vlist_trans[v0].y = y01i;
                    cur_face->vlist_trans[v0].z = -camera->near;
                    
                    // now comes the hard part, we have to carefully create a new polygon
                    // from the 2 intersection points and v2, this polygon will be inserted
                    // at the end of the rendering list, but for now, we are building it up
                    // in  temp_poly
                    
                    // so leave v2 alone, but overwrite v1 with v01, and overwrite v0 with v02
                    temp_poly.vlist_trans[v1].x = x01i;
                    temp_poly.vlist_trans[v1].y = y01i;
                    temp_poly.vlist_trans[v1].z = -camera->near;
                    
                    temp_poly.vlist_trans[v0].x = x02i;
                    temp_poly.vlist_trans[v0].y = y02i;
                    temp_poly.vlist_trans[v0].z = -camera->near;
                    
                    // 如果有纹理,则对纹理进行处理
                    if (cur_face->mati.mati_type & Material::TEXTURE) {
                        // compute poly 1 new texture coordinates from split
                        float u01i = cur_face->vlist_trans[v0].tu + (cur_face->vlist_trans[v1].tu - cur_face->vlist_trans[v0].tu)*t1;
                        float v01i = cur_face->vlist_trans[v0].tv + (cur_face->vlist_trans[v1].tv - cur_face->vlist_trans[v0].tv)*t1;
                        
                        // compute poly 2 new texture coordinates from split
                        float u02i = cur_face->vlist_trans[v0].tu + (cur_face->vlist_trans[v2].tu - cur_face->vlist_trans[v0].tu)*t2;
                        float v02i = cur_face->vlist_trans[v0].tv + (cur_face->vlist_trans[v2].tv - cur_face->vlist_trans[v0].tv)*t2;
                        
                        // write them all at the same time
                        // poly 1
                        cur_face->vlist_trans[v0].tu = u01i;
                        cur_face->vlist_trans[v0].tv = v01i;
                        
                        // poly 2
                        temp_poly.vlist_trans[v0].tu = u02i;
                        temp_poly.vlist_trans[v0].tv = v02i;
                        temp_poly.vlist_trans[v1].tu = u01i;
                        temp_poly.vlist_trans[v1].tv = v01i;
                    }
                    
                    // 再插入一个到 渲染列表
                    insertFace(&temp_poly);
                }
            }
        }
    }
}


#pragma mark 对 renderList 执行光照计算

void RenderList::light(Camera* camera){
    
    LightManager* lightManager = LightManager::getInstance();
    
    // for each valid poly,light it...
    for (int i = 0; i < num_faces; ++i) {
        Face* cur_face = &face_data[i];
        
        if (!(cur_face->state & FACE_STATE_ACTIVITY) ||
            (cur_face->state & FACE_STATE_BACKFACE) ||
            (cur_face->state & FACE_STATE_CLIPPED)) {
            continue;
        }
        
        // start flat  shader
        if (cur_face->mati.mati_type & Material::FLAT) {
            
            int r_sum = 0;
            int g_sum = 0;
            int b_sum = 0;
            
            
            for (int cur_light = 0; cur_light < lightManager->num_light; ++cur_light) {
                
                Light* light = &lightManager->mLights[cur_light];
                
                if (light->state == LIGHT_STATE_OFF) {
                    continue;
                }
                
                if (light->attr & LIGHT_ATTR_AMBIENT) {
                    
                    r_sum += (cur_face->lit_color[0].r * light->c_ambient.r / 256);
                    g_sum += (cur_face->lit_color[0].g * light->c_ambient.g / 256);
                    b_sum += (cur_face->lit_color[0].b * light->c_ambient.b / 256);
                    
                }else if (light->attr & LIGHT_ATTR_DIRECTIONAL){
                    
                    // 散射光照射结果 = 材质反射系数 * 散射光强度 * max(cos(入射角),0);
                    // 镜面反射光照射结果 = 材质反射系数 * 镜面光强度 * max(0,(cos(半向量与法向量夹角))^粗糙度)
                    
                    // 计算面法向量
                    // v0->v1   v0->v2
                    Vec3 v01 = cur_face->vlist_trans[1].v - cur_face->vlist_trans[0].v;
                    Vec3 v02 = cur_face->vlist_trans[2].v - cur_face->vlist_trans[0].v;
                    Vec3 vn;
                    Vec3::cross(v01,v02,&vn);
                    vn.normalize();
                    
                    float dp = Vec3::dot(vn,-light->trans_dir);
                    
                    if (dp > 0) {
                        r_sum += (light->c_diffuse.r * cur_face->lit_color[0].r * dp / 256);
                        g_sum += (light->c_diffuse.g * cur_face->lit_color[0].g * dp / 256);
                        b_sum += (light->c_diffuse.b * cur_face->lit_color[0].b * dp / 256);
                    }
                }else if (light->attr & LIGHT_ATTR_POINT){
                    
                    // perform point light computations
                    // light model for point light is once again:
                    //              I0point * Clpoint
                    //  I(d)point = ___________________
                    //              kc +  kl*d + kq*d2
                    //
                    //  Where d = |p - s|
                    // thus it's almost identical to the infinite light, but attenuates as a function
                    // of distance from the point source to the surface point being lit
                    
                    // 点光源
                    // 计算法向量
                    // v0->v1   v0->v2
                    Vec3 v01 = cur_face->vlist_trans[1].v - cur_face->vlist_trans[0].v;
                    Vec3 v02 = cur_face->vlist_trans[2].v - cur_face->vlist_trans[0].v;
                    Vec3 vn;
                    Vec3::cross(v01,v02,&vn);
                    vn.normalize();
                    
                    // compute vector from surface to light
                    // point->light_pos
                    Vec3 pl = light->trans_pos - cur_face->vlist_trans[0].v;
                    float dist = pl.length();
                    float dp = Vec3::dot(vn,pl);
                    
                    if (dp > 0) {
                        float atten = 1.0/(light->kc + light->kl * dist + light->kq * dist * dist);
                        float i = dp * atten / dist;
                        r_sum += (light->c_diffuse.r * cur_face->lit_color[0].r * i )/256;
                        g_sum += (light->c_diffuse.g * cur_face->lit_color[0].g * i )/256;
                        b_sum += (light->c_diffuse.b * cur_face->lit_color[0].b * i )/256;
                    }
                }else if (light->attr & LIGHT_ATTR_SPOTLIGHT1){
                    // 简单聚光灯
                    // 计算法向量
                    // v0->v1   v0->v2
                    Vec3 v01 = cur_face->vlist_trans[1].v - cur_face->vlist_trans[0].v;
                    Vec3 v02 = cur_face->vlist_trans[2].v - cur_face->vlist_trans[0].v;
                    Vec3 vn;
                    Vec3::cross(v01,v02,&vn);
                    vn.normalize();
                    
                    // compute vector from surface to light
                    // point->light_pos
                    Vec3 pl = light->trans_pos - cur_face->vlist_trans[0].v;
                    float dist = pl.length();
                    float dp = Vec3::dot(vn,pl);
                    
                    if (dp > 0) {
                        float atten = 1.0/(light->kc + light->kl * dist + light->kq * dist * dist);
                        float i = dp * atten;
                        r_sum += (light->c_diffuse.r * cur_face->lit_color[0].r *  i)/256;
                        g_sum += (light->c_diffuse.g * cur_face->lit_color[0].g *  i)/256;
                        b_sum += (light->c_diffuse.b * cur_face->lit_color[0].b *  i)/256;
                    }
                }
                
                // make sure colors aren't out of range
                if (r_sum  > 255) r_sum = 255;
                if (g_sum  > 255) g_sum = 255;
                if (b_sum  > 255) b_sum = 255;
                
                cur_face->lit_color[0].r = r_sum;
                cur_face->lit_color[0].g = g_sum;
                cur_face->lit_color[0].b = b_sum;
            }
            
        }// end of flat shader
        else if (cur_face->mati.mati_type & Material::GOURAUD){
            
            int r_sum0  = 0;
            int g_sum0  = 0;
            int b_sum0  = 0;
            
            int r_sum1  = 0;
            int g_sum1  = 0;
            int b_sum1  = 0;
            
            int r_sum2  = 0;
            int g_sum2  = 0;
            int b_sum2  = 0;
            
            for (int cur_light = 0; cur_light < lightManager->num_light; ++cur_light) {
                Light* light = &lightManager->mLights[cur_light];
                
                if (light->state == LIGHT_STATE_OFF) {
                    continue;
                }
                
                if (light->attr & LIGHT_ATTR_AMBIENT) {
                    
                    // 环境光
                    r_sum0 += light->c_ambient.r * cur_face->lit_color[0].r / 256;
                    g_sum0 += light->c_ambient.g * cur_face->lit_color[0].g / 256;
                    b_sum0 += light->c_ambient.b * cur_face->lit_color[0].b / 256;
                    
                    r_sum1 += light->c_ambient.r * cur_face->lit_color[1].r / 256;
                    g_sum1 += light->c_ambient.g * cur_face->lit_color[1].g / 256;
                    b_sum1 += light->c_ambient.b * cur_face->lit_color[1].b / 256;
                    
                    r_sum2 += light->c_ambient.r * cur_face->lit_color[2].r / 256;
                    g_sum2 += light->c_ambient.g * cur_face->lit_color[2].g / 256;
                    b_sum2 += light->c_ambient.b * cur_face->lit_color[2].b / 256;
                    
                }else if (light->attr & LIGHT_ATTR_DIRECTIONAL){
                    
                    // 方向光
                    // vertex0
                    float dp = Vec3::dot(cur_face->vlist_trans[0].n,-light->trans_dir);
                    
                    if (dp > 0) {
                        r_sum0 += (light->c_diffuse.r * cur_face->lit_color[0].r * dp)/256;
                        g_sum0 += (light->c_diffuse.g * cur_face->lit_color[0].g * dp)/256;
                        b_sum0 += (light->c_diffuse.b * cur_face->lit_color[0].b * dp)/256;
                    }
                    
                    // vertex1
                    dp = Vec3::dot(cur_face->vlist_trans[1].n,-light->trans_dir);
                    if (dp > 0) {
                        r_sum1 += (light->c_diffuse.r * cur_face->lit_color[1].r * dp)/256;
                        g_sum1 += (light->c_diffuse.g * cur_face->lit_color[1].g * dp)/256;
                        b_sum1 += (light->c_diffuse.b * cur_face->lit_color[1].b * dp)/256;
                    }
                    
                    // vertex2
                    dp = Vec3::dot(cur_face->vlist_trans[2].n,-light->trans_dir);
                    if (dp > 0) {
                        r_sum2 += (light->c_diffuse.r * cur_face->lit_color[2].r * dp)/256;
                        g_sum2 += (light->c_diffuse.g * cur_face->lit_color[2].g * dp)/256;
                        b_sum2 += (light->c_diffuse.b * cur_face->lit_color[2].b * dp)/256;
                    }
                }else if (light->attr & LIGHT_ATTR_POINT){
                    
                    // compute vector from surface to light
                    // vertex0
                    Vec3 pl = light->trans_pos - cur_face->vlist_trans[0].v;
                    float dist = pl.length();
                    
                    float dp = Vec3::dot(cur_face->vlist_trans[0].n,pl);
                    if (dp > 0) {
                        
                        float atten = 1.0/(light->kc + light->kl * dist + light->kq * dist * dist);
                        float i = dp * atten / dist;
                        
                        r_sum0 += (light->c_diffuse.r * cur_face->lit_color[0].r * i)/256;
                        g_sum0 += (light->c_diffuse.g * cur_face->lit_color[0].g * i)/256;
                        b_sum0 += (light->c_diffuse.b * cur_face->lit_color[0].b * i)/256;
                    }
                    
                    //vertex1
                    pl = light->trans_pos - cur_face->vlist_trans[1].v;
                    dist = pl.length();
                    
                    dp  = Vec3::dot(cur_face->vlist_trans[1].n,pl);
                    if (dp > 0) {
                        float atten = 1.0/(light->kc + light->kl * dist + light->kq * dist * dist);
                        float i = dp * atten / dist;
                        
                        r_sum1 += (light->c_diffuse.r * cur_face->lit_color[1].r * i)/256;
                        g_sum1 += (light->c_diffuse.g * cur_face->lit_color[1].g * i)/256;
                        b_sum1 += (light->c_diffuse.b * cur_face->lit_color[1].b * i)/256;
                    }
                    
                    //vertex2
                    pl = light->trans_pos - cur_face->vlist_trans[2].v;
                    dist = pl.length();
                    
                    dp  = Vec3::dot(cur_face->vlist_trans[2].n,pl);
                    if (dp > 0) {
                        float atten = 1.0/(light->kc + light->kl * dist + light->kq * dist * dist);
                        float i = dp * atten / dist;
                        
                        r_sum2 += (light->c_diffuse.r * cur_face->lit_color[2].r * i)/256;
                        g_sum2 += (light->c_diffuse.g * cur_face->lit_color[2].g * i)/256;
                        b_sum2 += (light->c_diffuse.b * cur_face->lit_color[2].b * i)/256;
                    }
                }else if (light->attr & LIGHT_ATTR_SPOTLIGHT1){
                    
                    // 简单聚光灯
                    // compute vector from surface to light
                    // vertex0
                    Vec3 pl = light->trans_pos - cur_face->vlist_trans[0].v;
                    float dist = pl.length();
                    
                    float dp = Vec3::dot(cur_face->vlist_trans[0].n,pl);
                    if (dp > 0) {
                        
                        float atten = 1.0/(light->kc + light->kl * dist + light->kq * dist * dist);
                        float i = dp * atten;
                        
                        r_sum0 += (light->c_diffuse.r * cur_face->lit_color[0].r * i)/256;
                        g_sum0 += (light->c_diffuse.g * cur_face->lit_color[0].g * i)/256;
                        b_sum0 += (light->c_diffuse.b * cur_face->lit_color[0].b * i)/256;
                    }
                    
                    //vertex1
                    pl = light->trans_pos - cur_face->vlist_trans[1].v;
                    dist = pl.length();
                    
                    dp  = Vec3::dot(cur_face->vlist_trans[1].n,pl);
                    if (dp > 0) {
                        float atten = 1.0/(light->kc + light->kl * dist + light->kq * dist * dist);
                        float i = dp * atten;
                        
                        r_sum1 += (light->c_diffuse.r * cur_face->lit_color[1].r * i)/256;
                        g_sum1 += (light->c_diffuse.g * cur_face->lit_color[1].g * i)/256;
                        b_sum1 += (light->c_diffuse.b * cur_face->lit_color[1].b * i)/256;
                    }
                    
                    //vertex2
                    pl = light->trans_pos - cur_face->vlist_trans[2].v;
                    dist = pl.length();
                    
                    dp  = Vec3::dot(cur_face->vlist_trans[2].n,pl);
                    if (dp > 0) {
                        float atten = 1.0/(light->kc + light->kl * dist + light->kq * dist * dist);
                        float i = dp * atten;
                        
                        r_sum2 += (light->c_diffuse.r * cur_face->lit_color[2].r * i)/256;
                        g_sum2 += (light->c_diffuse.g * cur_face->lit_color[2].g * i)/256;
                        b_sum2 += (light->c_diffuse.b * cur_face->lit_color[2].b * i)/256;
                    }
                }
            }
            
            // make sure colors aren't out of range
            if (r_sum0  > 255) r_sum0 = 255;
            if (g_sum0  > 255) g_sum0 = 255;
            if (b_sum0  > 255) b_sum0 = 255;
            
            if (r_sum1  > 255) r_sum1 = 255;
            if (g_sum1  > 255) g_sum1 = 255;
            if (b_sum1  > 255) b_sum1 = 255;
            
            if (r_sum2  > 255) r_sum2 = 255;
            if (g_sum2  > 255) g_sum2 = 255;
            if (b_sum2  > 255) b_sum2 = 255;
            
            cur_face->lit_color[0] = Color(r_sum0, g_sum0, b_sum0);
            cur_face->lit_color[1] = Color(r_sum1, g_sum1, b_sum1);
            cur_face->lit_color[2] = Color(r_sum2, g_sum2, b_sum2);
        }// end of gouraud
    }
}


void RenderList::perspective(Camera* camera){
    
    float tmpz = 0;
    
    for (int i = 0; i < num_faces; ++i) {
        Face* cur_face = &face_data[i];
        
        if (!(cur_face->state & FACE_STATE_ACTIVITY) ||
            (cur_face->state & FACE_STATE_BACKFACE) ||
            (cur_face->state & FACE_STATE_CLIPPED)) {
            continue;
        }
        
        // 将齐次坐标转换成 非齐次坐标
        for (int vertex = 0; vertex < 3; ++vertex) {
            tmpz = -1.0/cur_face->vlist_trans[vertex].z; 
            camera->mper.transformPoint(cur_face->vlist_trans[vertex].v,&cur_face->vlist_trans[vertex].v);
            cur_face->vlist_trans[vertex].v *= tmpz;
        }
    }
}


// 变换到屏幕坐标
void RenderList::toscreen(Camera* camera){
    for (int i = 0; i < num_faces; ++i) {
        Face* cur_face = &face_data[i];
        
        if (!(cur_face->state & FACE_STATE_ACTIVITY) ||
            (cur_face->state & FACE_STATE_BACKFACE) ||
            (cur_face->state & FACE_STATE_CLIPPED)) {
            continue;
        }
        
        // 将齐次坐标转换成 非齐次坐标
        for (int vertex = 0; vertex < 3; ++vertex) {
            camera->mscr.transformPoint(cur_face->vlist_trans[vertex].v,&cur_face->vlist_trans[vertex].v);
        }
    }
}

// 绘制线框
void RenderList::drawWire(){
    for (int i = 0; i < num_faces; ++i) {
        Face* cur_face = &face_data[i];
        
        if (!(cur_face->state & FACE_STATE_ACTIVITY) ||
            (cur_face->state & FACE_STATE_BACKFACE) ||
            (cur_face->state & FACE_STATE_CLIPPED)) {
            continue;
        }
        
        Uint32 color = cur_face->lit_color[0].toInt_RGB();
        draw_line_v1(cur_face->vlist_trans[0].x, cur_face->vlist_trans[0].y, cur_face->vlist_trans[1].x,cur_face->vlist_trans[1].y,color);
        draw_line_v1(cur_face->vlist_trans[1].x, cur_face->vlist_trans[1].y, cur_face->vlist_trans[2].x,cur_face->vlist_trans[2].y,color);
        draw_line_v1(cur_face->vlist_trans[2].x, cur_face->vlist_trans[2].y, cur_face->vlist_trans[0].x,cur_face->vlist_trans[0].y,color);
#ifdef _SD_DEBUG_
        ++gRenderFaceCount;
#endif
    }
}

// 绘制实体
void RenderList::drawSolid(){
    for (int i = 0; i < num_faces; ++i) {
        Face* cur_face = &face_data[i];
        
        if (!(cur_face->state & FACE_STATE_ACTIVITY) ||
            (cur_face->state & FACE_STATE_BACKFACE) ||
            (cur_face->state & FACE_STATE_CLIPPED)) {
            continue;
        }
        
        if (cur_face->mati.mati_type & Material::FLAT) {
            Uint32 color = cur_face->lit_color[0].toInt_RGB();
            draw_fill_triangle_v1(cur_face->vlist_trans[0].x, cur_face->vlist_trans[0].y,cur_face->vlist_trans[1].x, cur_face->vlist_trans[1].y, cur_face->vlist_trans[2].x, cur_face->vlist_trans[2].y, color);
        }else if (cur_face->mati.mati_type & Material::GOURAUD){
            draw_gouraud_triangle(cur_face);
        }
        

#ifdef _SD_DEBUG_
        ++gRenderFaceCount;
#endif
    }
}

void RenderList::draw(){
    
    for (int i = 0; i < num_faces; ++i) {
        Face* cur_face = &face_data[i];
        
        if (!(cur_face->state & FACE_STATE_ACTIVITY) ||
            (cur_face->state & FACE_STATE_BACKFACE) ||
            (cur_face->state & FACE_STATE_CLIPPED)) {
            continue;
        }
        
        if (cur_face->mati.mati_type == Material::WIRE) {

#if 0
            Uint32 color = cur_face->lit_color[0].toInt_RGB();
            draw_line_v1(cur_face->vlist_trans[0].x, cur_face->vlist_trans[0].y, cur_face->vlist_trans[1].x,cur_face->vlist_trans[1].y,color);
            draw_line_v1(cur_face->vlist_trans[1].x, cur_face->vlist_trans[1].y, cur_face->vlist_trans[2].x,cur_face->vlist_trans[2].y,color);
            draw_line_v1(cur_face->vlist_trans[2].x, cur_face->vlist_trans[2].y, cur_face->vlist_trans[0].x,cur_face->vlist_trans[0].y,color);
#endif
            Uint32 color = cur_face->lit_color[0].toInt_RGB();
            DrawLine(cur_face->vlist_trans[0].x, cur_face->vlist_trans[0].y, cur_face->vlist_trans[1].x,cur_face->vlist_trans[1].y,color);
            DrawLine(cur_face->vlist_trans[1].x, cur_face->vlist_trans[1].y, cur_face->vlist_trans[2].x,cur_face->vlist_trans[2].y,color);
            DrawLine(cur_face->vlist_trans[2].x, cur_face->vlist_trans[2].y, cur_face->vlist_trans[0].x,cur_face->vlist_trans[0].y,color);
            
        }else if (cur_face->mati.mati_type == Material::FLAT||
                  cur_face->mati.mati_type == Material::CONSTANT){
            
#if 0
            Uint32 color = cur_face->lit_color[0].toInt_RGB();
            draw_fill_triangle_v1(cur_face->vlist_trans[0].x, cur_face->vlist_trans[0].y,cur_face->vlist_trans[1].x, cur_face->vlist_trans[1].y, cur_face->vlist_trans[2].x, cur_face->vlist_trans[2].y, color);
#endif
            
            DrawTriangle(cur_face);
            
        }else if (cur_face->mati.mati_type == Material::GOURAUD){
            
            draw_gouraud_triangle(cur_face);
            
        }else if (cur_face->mati.mati_type == Material::TEXTURE){
            
            draw_texture_triangle(cur_face);
            
        }
        
#ifdef _SD_DEBUG_
        ++gRenderFaceCount;
#endif
    }
}


////////////////////////////////////////////////////////////////////////////////

int RenderList::compare_avgz_face(const void *arg1, const void *arg2)
{
    // this function comapares the average z's of two polygons and is used by the
    // depth sort surface ordering algorithm
    
    float z1, z2;
    
    Face* poly_1 = (Face*)arg1;
    Face* poly_2 = (Face*)arg2;
    
    // compute z average of each polygon
    z1 = (float)0.33333*(poly_1->vlist_trans[0].z + poly_1->vlist_trans[1].z + poly_1->vlist_trans[2].z);
    
    // now polygon 2
    z2 = (float)0.33333*(poly_2->vlist_trans[0].z + poly_2->vlist_trans[1].z + poly_2->vlist_trans[2].z);
    
    // compare z1 and z2, such that polys' will be sorted in descending Z order
    if (z1 > z2){
        return(-1);
    }else{
        if (z1 < z2){
             return(1);
        }
        else{
            return(0);
        }
    }
} // end Compare_AvgZ_POLYF4DV2


////////////////////////////////////////////////////////////////////////////////

int RenderList::compare_nearz_face(const void *arg1, const void *arg2)
{
    // this function comapares the closest z's of two polygons and is used by the
    // depth sort surface ordering algorithm
    
    float z1, z2;
    
    Face* poly_1 = (Face*)arg1;
    Face* poly_2 = (Face*)arg2;
    
    // compute the near z of each polygon
    z1 = MIN(poly_1->vlist_trans[0].z, poly_1->vlist_trans[1].z);
    z1 = MIN(z1, poly_1->vlist_trans[2].z);
    
    z2 = MIN(poly_2->vlist_trans[0].z, poly_2->vlist_trans[1].z);
    z2 = MIN(z2, poly_2->vlist_trans[2].z);
    
    // compare z1 and z2, such that polys' will be sorted in descending Z order
    if (z1 > z2)
        return(-1);
    else
        if (z1 < z2)
            return(1);
        else
            return(0);
    
} // end Compare_NearZ_POLYF4DV2

////////////////////////////////////////////////////////////////////////////////

int RenderList::compare_farz_face(const void *arg1, const void *arg2)
{
    // this function comapares the farthest z's of two polygons and is used by the
    // depth sort surface ordering algorithm
    
    float z1, z2;
    
    Face* poly_1 = (Face*)arg1;
    Face* poly_2 = (Face*)arg2;

    
    // compute the near z of each polygon
    z1 = MAX(poly_1->vlist_trans[0].z, poly_1->vlist_trans[1].z);
    z1 = MAX(z1, poly_1->vlist_trans[2].z);
    
    z2 = MAX(poly_2->vlist_trans[0].z, poly_2->vlist_trans[1].z);
    z2 = MAX(z2, poly_2->vlist_trans[2].z);
    
    // compare z1 and z2, such that polys' will be sorted in descending Z order
    if (z1 > z2)
        return(-1);
    else
        if (z1 < z2)
            return(1);
        else
            return(0);
    
} // end Compare_FarZ_POLYF4DV2

void RenderList::sort(int sort_method){
    if (sort_method == SORT_FACE_AVGZ) {
        qsort((void*)face_data,num_faces,sizeof(Face),RenderList::compare_avgz_face);
    }else if (sort_method == SORT_FACE_NEARZ){
        qsort((void*)face_data,num_faces,sizeof(Face),RenderList::compare_nearz_face);
    }else if (sort_method == SORT_FACE_FARZ){
        qsort((void*)face_data,num_faces,sizeof(Face),RenderList::compare_farz_face);
    }
}



