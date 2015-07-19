//
//  RenderTriangle3D.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-24.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "RenderTriangle3D.h"
#include "RenderTriangle2D.h"

enum class TRIANGLE_TYPE{
    
    // 平顶
    FLAT_TOP_TRIANGLE = 1,
    
    // 平底
    FLAT_BOTTOM_TRIANGLE,
    
    // 左边长
    LEFT_LONG_TRIANGLE,
    
    // 右边长
    RIGHT_LONG_TRIANGLE,
    
};

#pragma mark 进行透视修正纹理

void RenderTriangleTexture3D(Face* face){
    
    // first trivial clipping rejection tests
    if (((face->vlist_trans[0].y < kMIN_CLIP_Y)  &&
         (face->vlist_trans[1].y < kMIN_CLIP_Y)  &&
         (face->vlist_trans[2].y < kMIN_CLIP_Y)) ||
        
        ((face->vlist_trans[0].y > kMAX_CLIP_Y)  &&
         (face->vlist_trans[1].y > kMAX_CLIP_Y)  &&
         (face->vlist_trans[2].y > kMAX_CLIP_Y)) ||
        
        ((face->vlist_trans[0].x < kMIN_CLIP_X)  &&
         (face->vlist_trans[1].x < kMIN_CLIP_X)  &&
         (face->vlist_trans[2].x < kMIN_CLIP_X)) ||
        
        ((face->vlist_trans[0].x > kMAX_CLIP_X)  &&
         (face->vlist_trans[1].x > kMAX_CLIP_X)  &&
         (face->vlist_trans[2].x > kMAX_CLIP_X)))
    {
        return;
    }
    
    
    // degenerate triangle
    if ( ((face->vlist_trans[0].x==face->vlist_trans[1].x) &&
          (face->vlist_trans[1].x==face->vlist_trans[2].x)) ||
        ((face->vlist_trans[0].y==face->vlist_trans[1].y) &&
         (face->vlist_trans[1].y==face->vlist_trans[2].y)))
    {
        return;
    }
    
    // 排列顶点
    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp;
    if (face->vlist_trans[v1].y < face->vlist_trans[v0].y) {
        temp = v0;
        v0 = v1;
        v1 = temp;
    }
    
    if (face->vlist_trans[v2].y < face->vlist_trans[v0].y){
        temp = v0;
        v0 = v2;
        v2 = temp;
    }
    
    if (face->vlist_trans[v2].y < face->vlist_trans[v1].y) {
        temp = v1;
        v1 = v2;
        v2 = temp;
    }
    
    TRIANGLE_TYPE tri_type;
    
    // 检测是哪种三角形
    if (face->vlist_trans[v0].y == face->vlist_trans[v1].y) {
        // 平顶三角形
        tri_type = TRIANGLE_TYPE::FLAT_TOP_TRIANGLE;
        
        // 从左到右排列顶点
        if (face->vlist_trans[v1].x < face->vlist_trans[v0].x) {
            temp = v0;
            v0 = v1;
            v1 = temp;
        }
    }//end if
    else{
        if (face->vlist_trans[v1].y == face->vlist_trans[v2].y) {
            // 平底三角形
            tri_type = TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE;
            
            //从左到右
            if (face->vlist_trans[v1].x > face->vlist_trans[v2].x) {
                temp = v1;
                v1 = v2;
                v2 = temp;
            }
        }// end if
        else{
            // 判断三角形是左边长还是右边长
            // 插值计算
            float interx = face->vlist_trans[v0].x + (face->vlist_trans[v1].y - face->vlist_trans[v0].y)*(face->vlist_trans[v2].x - face->vlist_trans[v0].x)/(face->vlist_trans[v2].y - face->vlist_trans[v0].y);
            if (interx > face->vlist_trans[v1].x) {
                tri_type = TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE;
            }else{
                tri_type = TRIANGLE_TYPE::LEFT_LONG_TRIANGLE;
            }
        }// end else
    }// end else
    
    float x0 = face->vlist_trans[v0].x;
    float y0 = face->vlist_trans[v0].y;
    float z0 = face->vlist_trans[v0].z;
    
    float x1 = face->vlist_trans[v1].x;
    float y1 = face->vlist_trans[v1].y;
    float z1 = face->vlist_trans[v1].z;
    
    float x2 = face->vlist_trans[v2].x;
    float y2 = face->vlist_trans[v2].y;
    float z2 = face->vlist_trans[v2].z;
    
    Uint32* dst_buffer = (Uint32*)gSurface->pixels;
    int mem_pitch = gSurface->pitch >> 2;
    
    float s0 = face->vlist_trans[v0].tu;
    float t0 = face->vlist_trans[v0].tv;
    
    float s1 = face->vlist_trans[v1].tu;
    float t1 = face->vlist_trans[v1].tv;
    
    float s2 = face->vlist_trans[v2].tu;
    float t2 = face->vlist_trans[v2].tv;
    
    Uint32* texmap = (Uint32*)face->mati.texture->surface->pixels;
    int tex_pitch = face->mati.texture->surface->pitch >> 2;
    
    int minTexY = 0;
    int minTexX = 0;
    int maxTexY = face->mati.texture->h - 1;
    int maxTexX = face->mati.texture->w - 1;
    
//    float invz0 = z0;
//    float invz1 = z1;
//    float invz2 = z2;
    
    float invz0 = 1.0/z0;
    float invz1 = 1.0/z1;
    float invz2 = 1.0/z2;
    
    float invz_s0 = s0/z0;
    float invz_t0 = t0/z0;
    
    float invz_s1 = s1/z1;
    float invz_t1 = t1/z1;
    
    float invz_s2 = s2/z2;
    float invz_t2 = t2/z2;
    
    // 提取顶点,准备进行光栅化
#pragma mark 处理 FLAT_TOP 三角形
    if (tri_type == TRIANGLE_TYPE::FLAT_TOP_TRIANGLE) {
        
//        int start = (int)y0 * mem_pitch;
//        int endY = y2;
//        float xl,xr;
//        
//        for (int y = y0; y < endY; ++y) {
//            xl = x0 + (y-y0)*(x2-x0)/(y2-y0);
//            xr = x1 + (y-y1)*(x2-x1)/(y2-y1);
//            
//            float oneoverz_left = (y-y0)*(invz2 - invz0)/(y2 - y0) + invz0;
//            float oneoverz_right = (y-y1)*(invz2 - invz1)/(y2-y1) + invz1;
//            float oneoverz_step = (oneoverz_right - oneoverz_left)/(xr - xl);
//            
//            float soverz_left = (y - y0) * (invz_s2 - invz_s0)/(y2 - y0) + invz_s0;
//            float soverz_right = (y - y1)*(invz_s2 - invz_s1)/(y2 - y1) + invz_s1;
//            
//            float toverz_left = (y - y0)*(invz_t2 - invz_t0)/(y2 - y0) + invz_t0;
//            float toverz_right = (y - y1)*(invz_t2 - invz_t1)/(y2 - y1) + invz_t1;
//            
//            float soverz_step = (soverz_right - soverz_left)/(xr - xl);
//            float toverz_step = (toverz_right - toverz_left)/(xr - xl);
//            
//            float oneoverz = oneoverz_left;
//            float soverz = soverz_left;
//            float toverz = toverz_left;
//            for (int x = xl;x < xr;++x) {
//                float s = soverz/oneoverz;
//                float t = toverz/oneoverz;
//                dst_buffer[start + x] = texmap[clampInt(minTexY,maxTexY,t) * tex_pitch + clampInt(minTexX,maxTexX,s)];
//                oneoverz += oneoverz_step;
//                soverz += soverz_step;
//                toverz += toverz_step;
//            }
//            start += mem_pitch;
//        }
        

        float dy = y2 - y0;
        float dxdyl = (x2 - x0)/dy;
        float dxdyr = (x2 - x1)/dy;
        
        float dinvzdyl = (invz2 - invz0)/dy;
        float dinvzdyr = (invz2 - invz1)/dy;
        
        // IL
        float dszdyl = (invz_s2 - invz_s0)/dy;
        float dtzdyl = (invz_t2 - invz_t0)/dy;
        
        // IR
        float dszdyr = (invz_s2 - invz_s1)/dy;
        float dtzdyr = (invz_t2 - invz_t1)/dy;
        
        // 起始
        float xs_invzs = invz_s0;
        float xs_invzt = invz_t0;
        
        float xe_invzs = invz_s1;
        float xe_invzt = invz_t1;
        
        float xs_invz = invz0;
        float xe_invz = invz1;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            
            xs_invz += dinvzdyl * dy2;
            xe_invz += dinvzdyr * dy2;
            
            xs_invzs += dszdyl * dy2;
            xs_invzt += dtzdyl * dy2;
            
            xe_invzs += dszdyr * dy2;
            xe_invzt += dtzdyr * dy2;
        }
        
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }
        
        int endY = y2;
        float xs = x0,xe = x1;
        float old_xs = x0,old_xe = x1;
        int start = (int)y0 * mem_pitch;
        
        // 需要用到的变量
        float dx;
        float dinvzdx;
        float dinvsdx,dinvtdx;
        float xs_invzx;
        
        for (int i = y0; i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                
                xs_invz += dinvzdyl;
                xe_invz += dinvzdyr;
                
                xs_invzs += dszdyl;
                xs_invzt += dtzdyl;
                
                xe_invzs += dszdyr;
                xe_invzt += dtzdyr;
                
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            if (distx == 0.0) {
                dinvzdx = 0.0;
                dinvsdx = 0.0;
                dinvtdx = 0.0;
            }else{
                dinvzdx = (xe_invz  - xs_invz)/distx;
                dinvsdx = (xe_invzs - xs_invzs)/distx;
                dinvtdx = (xe_invzt - xs_invzt)/distx;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            
            xs_invzx = xs_invz + dx * dinvzdx;
            
            float begin_invzs = xs_invzs;
            float begin_invzt = xs_invzt;
            
            for (int k = xs; k <= xe; ++k) {
                
                float ss = begin_invzs/xs_invzx;
                float st = begin_invzt/xs_invzx;
                
                dst_buffer[start + k] = texmap[clampInt(minTexY,maxTexY,st) * tex_pitch + clampInt(minTexX,maxTexX,ss)];
                
                begin_invzs += dinvsdx;
                begin_invzt += dinvtdx;
                
                xs_invzx += dinvzdx;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
            
            // 相应的增量
            xs_invz += dinvzdyl;
            xe_invz += dinvzdyr;
            
            xs_invzs += dszdyl;
            xs_invzt += dtzdyl;
            
            xe_invzs += dszdyr;
            xe_invzt += dtzdyr;
        }
    }
#pragma mark 处理 FLAT_BOOTOM 三角形
    
    else if(tri_type == TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE){
        
//        int start = (int)y0 * mem_pitch;
//        int endY = y2;
//        float xl,xr;
//        
//        for (int y = y0; y < endY; ++y) {
//            xl = x0 + (y-y0)*(x1-x0)/(y1-y0);
//            xr = x0 + (y-y0)*(x2-x0)/(y2-y0);
//            
//            float oneoverz_left = (y-y0)*(invz1 - invz0)/(y1 - y0) + invz0;
//            float oneoverz_right = (y-y0)*(invz2 - invz0)/(y2-y0) + invz0;
//            float oneoverz_step = (oneoverz_right - oneoverz_left)/(xr - xl);
//            
//            float soverz_left = (y - y0) * (invz_s1 - invz_s0)/(y2 - y0) + invz_s0;
//            float soverz_right = (y - y0)*(invz_s2 - invz_s0)/(y2 - y0) + invz_s0;
//            
//            float toverz_left = (y - y0)*(invz_t2 - invz_t0)/(y2 - y0) + invz_t0;
//            float toverz_right = (y - y0)*(invz_t2 - invz_t0)/(y2 - y0) + invz_t0;
//            
//            float soverz_step = (soverz_right - soverz_left)/(xr - xl);
//            float toverz_step = (toverz_right - toverz_left)/(xr - xl);
//            
//            float oneoverz = oneoverz_left;
//            float soverz = soverz_left;
//            float toverz = toverz_left;
//            for (int x = xl;x < xr;++x) {
//                float s = soverz/oneoverz;
//                float t = toverz/oneoverz;
//                dst_buffer[start + x] = texmap[clampInt(minTexY,maxTexY,t) * tex_pitch + clampInt(minTexX,maxTexX,s)];
//                oneoverz += oneoverz_step;
//                soverz += soverz_step;
//                toverz += toverz_step;
//            }
//            start += mem_pitch;
//        }
        

        float dy = y1 - y0;
        float dxdyl = (x1-x0)/dy;
        float dxdyr = (x2-x0)/dy;
        
        float dinvzdyl = (invz1 - invz0)/dy;
        float dinvzdyr = (invz2 - invz0)/dy;
        
        //IL
        float dszdyl = (invz_s1 - invz_s0)/dy;
        float dtzdyl = (invz_t1 - invz_t0)/dy;
        
        //IR
        float dszdyr = (invz_s2 - invz_s0)/dy;
        float dtzdyr = (invz_t2 - invz_t0)/dy;
        
        // 起始
        float xs_invzs = invz_s0;
        float xs_invzt = invz_t0;
        
        float xe_invzs = invz_s0;
        float xe_invzt = invz_t0;
        
        float xs_invz = invz0;
        float xe_invz = invz0;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            
            xs_invz += dinvzdyl * dy2;
            xe_invz += dinvzdyr * dy2;
            
            xs_invzs += dszdyl * dy2;
            xs_invzt += dtzdyl * dy2;
            
            xe_invzs += dszdyr * dy2;
            xe_invzt += dtzdyr * dy2;
        }
        
        if (y1 > kMAX_CLIP_Y) {
            y1 = kMAX_CLIP_Y;
        }
        
        float xs = x0,xe = x0;
        float old_xs = x0,old_xe = x0;
        int endY = y1;
        int start = (int)y0 * mem_pitch;
        
        // 需要用到的变量
        float dx;
        float dinvzdx;
        float dinvsdx,dinvtdx;
        
        float xs_invzx;
        
        for (int i = y0; i < endY;++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                
                xs_invz += dinvzdyl;
                xe_invz += dinvzdyr;
                
                xs_invzs += dszdyl;
                xs_invzt += dtzdyl;
                
                xe_invzs += dszdyr;
                xe_invzt += dtzdyr;
                
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            if (distx == 0.0) {
                dinvzdx = 0.0;
                dinvsdx = 0.0;
                dinvtdx = 0.0;
            }else{
                dinvzdx = (xe_invz - xs_invz)/distx;
                dinvsdx = (xe_invzs - xs_invzs)/distx;
                dinvtdx = (xe_invzt - xs_invzt)/distx;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            
            xs_invzx = xs_invz + dx * dinvzdx;
            
            float begin_invzs = xs_invzs;
            float begin_invzt = xs_invzt;
            
            for (int k = xs; k <= xe; ++k) {
                float ss = begin_invzs/xs_invzx;
                float st = begin_invzt/xs_invzx;
                
                dst_buffer[start + k] = texmap[clampInt(minTexY,maxTexY,st) * tex_pitch + clampInt(minTexX,maxTexX,ss)];
                
                begin_invzs += dinvsdx;
                begin_invzt += dinvtdx;
                
                xs_invzx += dinvzdx;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
            
            // 相应的增量
            xs_invz += dinvzdyl;
            xe_invz += dinvzdyr;
            
            xs_invzs += dszdyl;
            xs_invzt += dtzdyl;
            
            xe_invzs += dszdyr;
            xe_invzt += dtzdyr;
        }
    }
    
#pragma mark 实现右边长的渲染
    
    else if (tri_type == TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE){
        
        //        int start = (int)y0 * mem_pitch;
        //        int endY = y1;
        //        float xl,xr;
        //
        //        for (int y = y0; y < endY; ++y) {
        //
        //            xl = x0 + (y - y0)*(x1-x0)/( y1 - y0);
        //            xr = x0 + (y - y0)*(x2-x0)/( y2 - y0);
        //
        //           float oneoverz_left = (y-y0)*(invz1 - invz0)/(y1 - y0) + invz0;
        //           float oneoverz_right = (y-y0)*(invz2 - invz0)/(y2-y0) + invz0;
        //           float oneoverz_step = (oneoverz_right - oneoverz_left)/(xr - xl);
        //
        //            float soverz_left = (y - y0) * (invz_s1 - invz_s0)/(y1 - y0) + invz_s0;
        //            float soverz_right = (y - y0)*(invz_s2 - invz_s0)/(y2 - y0) + invz_s0;
        //            float toverz_left = (y - y0)*(invz_t1 - invz_t0)/(y1 - y0) + invz_t0;
        //            float toverz_right = (y - y0)*(invz_t2 - invz_t0)/(y2 - y0) + invz_t0;
        //
        //            float soverz_step = (soverz_right - soverz_left)/(xr - xl);
        //            float toverz_step = (toverz_right - toverz_left)/(xr - xl);
        //
        //            float oneoverz = oneoverz_left;
        //            float soverz = soverz_left;
        //            float toverz = toverz_left;
        //            for (int x = xl;x < xr;++x) {
        //                float s = soverz/oneoverz;
        //                float t = toverz/oneoverz;
        //                dst_buffer[start + x] = texmap[clampInt(minTexY,maxTexY,t) * tex_pitch + clampInt(minTexX,maxTexX,s)];
        //                oneoverz += oneoverz_step;
        //                soverz += soverz_step;
        //                toverz += toverz_step;
        //            }
        //            start += mem_pitch;
        //        }
        //
        //        endY = y2;
        //        for (int y = y1;  y < y2; ++y) {
        //
        //            xl = x1 + (y - y1)*(x2-x1)/( y2 - y1);
        //            xr = x0 + (y - y0)*(x2-x0)/( y2 - y0);
        //
        //            float oneoverz_left = (y-y1)*(invz2 - invz1)/(y2 - y1) + invz1;
        //            float oneoverz_right = (y-y0)*(invz2 - invz0)/(y2-y0) + invz0;
        //            float oneoverz_step = (oneoverz_right - oneoverz_left)/(xr - xl);
        //
        //            float soverz_left = (y - y1) * (invz_s2 - invz_s1)/(y2 - y1) + invz_s1;
        //            float soverz_right = (y - y0)*(invz_s2 - invz_s0)/(y2 - y0) + invz_s0;
        //            float toverz_left = (y - y1)*(invz_t2 - invz_t1)/(y2 - y1) + invz_t1;
        //            float toverz_right = (y - y0)*(invz_t2 - invz_t0)/(y2 - y0) + invz_t0;
        //
        //            float soverz_step = (soverz_right - soverz_left)/(xr - xl);
        //            float toverz_step = (toverz_right - toverz_left)/(xr - xl);
        //
        //            float oneoverz = oneoverz_left;
        //            float soverz = soverz_left;
        //            float toverz = toverz_left;
        //            for (int x = xl;x < xr;++x) {
        //                float s = soverz/oneoverz;
        //                float t = toverz/oneoverz;
        //                dst_buffer[start + x] = texmap[clampInt(minTexY,maxTexY,t) * tex_pitch + clampInt(minTexX,maxTexX,s)];
        //                oneoverz += oneoverz_step;
        //                soverz += soverz_step;
        //                toverz += toverz_step;
        //            }
        //            start += mem_pitch;
        //        }
        
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        float dinvzdy10 = (invz1 - invz0)/dy10;
        float dinvzdy20 = (invz2 - invz0)/dy20;
        float dinvzdy21 = (invz2 - invz1)/dy21;
        
        //IL1
        float dszdy10 = (invz_s1 - invz_s0)/dy10;
        float dtzdy10 = (invz_t1 - invz_t0)/dy10;
        
        //IR
        float dszdy20 = (invz_s2 - invz_s0)/dy20;
        float dtzdy20 = (invz_t2 - invz_t0)/dy20;
        
        //IL2
        float dszdy21 = (invz_s2 - invz_s1)/dy21;
        float dtzdy21 = (invz_t2 - invz_t1)/dy21;
        
        // 起始
        float xs_invzs = invz_s0;
        float xs_invzt = invz_t0;
        
        float xe_invzs = invz_s0;
        float xe_invzt = invz_t0;
        
        float xs_invz = invz0;
        float xe_invz = invz0;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            
            xs_invz += dinvzdy10 * dy2;
            xe_invz += dinvzdy20 * dy2;
            
            xs_invzs += dszdy10 * dy2;
            xs_invzt += dtzdy10 * dy2;
            
            xe_invzs += dszdy20 * dy2;
            xe_invzt += dtzdy20 * dy2;
        }
        
        // test y1
        if (y1 < kMIN_CLIP_Y) {
            y1 = kMIN_CLIP_Y;
        }
        
        if (y1 > kMAX_CLIP_Y) {
            y1 = kMAX_CLIP_Y;
        }
        
        // test y2
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }
        
        
        float xs = x0,xe = x0;
        float old_xs = x0,old_xe = x0;
        int start = (int)y0 * mem_pitch;
        
        //分两段绘制
        int endY = y1;
        
        // 需要用到的变量
        float dx;
        float dinvzdx;
        float dinvsdx,dinvtdx;
        float xs_invzx;
        
        float xss,xst,dsdx,dtdx;
        float xes,xet;
        
        for (int i = y0; i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                
                old_xs += dxdy10;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                
                xs_invz += dinvzdy10;
                xe_invz += dinvzdy20;
                
                xs_invzs += dszdy10;
                xs_invzt += dtzdy10;
                
                xe_invzs += dszdy20;
                xe_invzt += dtzdy20;
                
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            if (distx == 0.0) {
                dinvzdx = 0.0;
                dinvsdx = 0.0;
                dinvtdx = 0.0;
            }else{
                dinvzdx = (xe_invz - xs_invz)/distx;
                dinvsdx = (xe_invzs - xs_invzs)/distx;
                dinvtdx = (xe_invzt - xs_invzt)/distx;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            
            xs_invzx = xs_invz + dx * dinvzdx;
            
            float begin_invzs = xs_invzs;
            float begin_invzt = xs_invzt;
            
            for (int k = xs; k <= xe; ++k) {
                float ss = begin_invzs/xs_invzx;
                float st = begin_invzt/xs_invzx;
                
                dst_buffer[start + k] = texmap[clampInt(minTexY,maxTexY,st) * tex_pitch + clampInt(minTexX,maxTexX,ss)];
                
                begin_invzs += dinvsdx;
                begin_invzt += dinvtdx;
                
                xs_invzx += dinvzdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy10;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
            
            // 相应的增量
            xs_invz += dinvzdy10;
            xe_invz += dinvzdy20;
            
            xs_invzs += dszdy10;
            xs_invzt += dtzdy10;
            
            xe_invzs += dszdy20;
            xe_invzt += dtzdy20;
        }
        
        endY = y2;
        xs = x1;
        old_xs = x1;
        
        xs_invz = invz1;
        xs_invzs = invz_s1;
        xs_invzt = invz_t1;
        
        for (int i = y1; i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy21;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                
                xs_invz += dinvzdy21;
                xe_invz += dinvzdy20;
                
                xs_invzs += dszdy21;
                xs_invzt += dtzdy21;
                
                xe_invzs += dszdy20;
                xe_invzt += dtzdy20;
                
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            if (distx == 0.0) {
                dinvzdx = 0.0;
                dinvsdx = 0.0;
                dinvtdx = 0.0;
            }else{
                dinvzdx = (xe_invz - xs_invz)/distx;
                dinvsdx = (xe_invzs - xs_invzs)/distx;
                dinvtdx = (xe_invzt - xs_invzt)/distx;
            }
            
            if (distx == 0.0) {
                dsdx = 0.0;
                dtdx = 0.0;
            }else{
                dsdx = (xes - xss)/distx;
                dtdx = (xet - xst)/distx;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            
            xs_invzx = xs_invz + dx * dinvzdx;
            
            float begin_invzs = xs_invzs;
            float begin_invzt = xs_invzt;
            
            for (int k = xs; k <= xe; ++k) {
                float ss = begin_invzs/xs_invzx;
                float st = begin_invzt/xs_invzx;
                
                dst_buffer[start + k] = texmap[clampInt(minTexY,maxTexY,st) * tex_pitch + clampInt(minTexX,maxTexX,ss)];
                
                begin_invzs += dinvsdx;
                begin_invzt += dinvtdx;
                
                xs_invzx += dinvzdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy21;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
            
            // 相应的增量
            xs_invz += dinvzdy21;
            xe_invz += dinvzdy20;
            
            xs_invzs += dszdy21;
            xs_invzt += dtzdy21;
            
            xe_invzs += dszdy20;
            xe_invzt += dtzdy20;
        }
    }
    
#pragma mark 实现左边长的渲染
    else if (tri_type == TRIANGLE_TYPE::LEFT_LONG_TRIANGLE){
        
        
        //        int start = (int)y0 * mem_pitch;
        //        int endY = y1;
        //        float xl,xr;
        //        for (int y = y0; y < endY; ++y) {
        //
        //            xl = x0 + (y - y0) * (x2 - x0)/(y2 - y0);
        //            xr = x0 + (y - y0) * (x1 - x0)/(y1 - y0);
        //
        //            float oneoverz_left = (y - y0)*(invz2 - invz0)/(y2-y0) + invz0;
        //            float oneoverz_right = (y - y0) * (invz1 - invz0)/(y1 - y0) + invz0;
        //            float oneoverz_step = (oneoverz_right - oneoverz_left)/(xr - xl);
        //
        //            float soverz_left = (y - y0)*(invz_s2 - invz_s0)/(y2 - y0) + invz_s0;
        //            float soverz_right = (y - y0)*(invz_s1 - invz_s0)/(y1 - y0) + invz_s0;
        //            float soverz_step = (soverz_right - soverz_left)/(xr - xl);
        //
        //            float toverz_left = (y - y0)*(invz_t2 - invz_t0)/(y2-y0) + invz_t0;
        //            float toverz_right = (y - y0)*(invz_t1 - invz_t0)/(y1-y0) + invz_t0;
        //            float toverz_step = (toverz_right - toverz_left)/(xr - xl);
        //
        //            float oneoverz = oneoverz_left;
        //            float soverz = soverz_left;
        //            float toverz = toverz_left;
        //            for (int x = xl;x < xr;++x) {
        //                float s = soverz/oneoverz;
        //                float t = toverz/oneoverz;
        //                dst_buffer[start + x] = texmap[clampInt(minTexY,maxTexY,t) * tex_pitch + clampInt(minTexX,maxTexX,s)];
        //                oneoverz += oneoverz_step;
        //                soverz += soverz_step;
        //                toverz += toverz_step;
        //            }
        //            start += mem_pitch;
        //        }
        //
        //        endY = y2;
        //        for (int y = y1;y < endY; ++y) {
        //            xl = x0 + (y - y0) * (x2 - x0)/(y2 - y0);
        //            xr = x1 + (y - y1) * (x2 - x1)/(y2 - y1);
        //
        //            float oneoverz_left = (y - y0)*(invz2 - invz0)/(y2-y0) + invz0;
        //            float oneoverz_right = (y - y1) * (invz2 - invz1)/(y2 - y1) + invz1;
        //            float oneoverz_step = (oneoverz_right - oneoverz_left)/(xr - xl);
        //
        //            float soverz_left = (y - y0)*(invz_s2 - invz_s0)/(y2 - y0) + invz_s0;
        //            float soverz_right = (y - y1)*(invz_s2 - invz_s1)/(y2 - y1) + invz_s1;
        //            float soverz_step = (soverz_right - soverz_left)/(xr - xl);
        //
        //            float toverz_left = (y - y0)*(invz_t2 - invz_t0)/(y2-y0) + invz_t0;
        //            float toverz_right = (y - y1)*(invz_t2 - invz_t1)/(y2-y1) + invz_t1;
        //            float toverz_step = (toverz_right - toverz_left)/(xr - xl);
        //
        //            float oneoverz = oneoverz_left;
        //            float soverz = soverz_left;
        //            float toverz = toverz_left;
        //            for (int x = xl;x < xr;++x) {
        //                float s = soverz/oneoverz;
        //                float t = toverz/oneoverz;
        //                dst_buffer[start + x] = texmap[clampInt(minTexY,maxTexY,t) * tex_pitch + clampInt(minTexX,maxTexX,s)];
        //                oneoverz += oneoverz_step;
        //                soverz += soverz_step;
        //                toverz += toverz_step;
        //            }
        //            start += mem_pitch;
        //        }
        
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        float dinvzdy10 = (invz1 - invz0)/dy10;
        float dinvzdy20 = (invz2 - invz0)/dy20;
        float dinvzdy21 = (invz2 - invz1)/dy21;
        
        // IL
        float dszdy20 = (invz_s2 - invz_s0)/dy20;
        float dtzdy20 = (invz_t2 - invz_t0)/dy20;
        
        //IR1
        float dszdy10 = (invz_s1 - invz_s0)/dy10;
        float dtzdy10 = (invz_t1 - invz_t0)/dy10;
        
        //IR2
        float dszdy21 = (invz_s2 - invz_s1)/dy21;
        float dtzdy21 = (invz_t2 - invz_t1)/dy21;
        
        // 起始
        float xs_invzs = invz_s0;
        float xs_invzt = invz_t0;
        
        float xe_invzs = invz_s0;
        float xe_invzt = invz_t0;
        
        float xs_invz = invz0;
        float xe_invz = invz0;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            
            xs_invz += dinvzdy20 * dy2;
            xe_invz += dinvzdy10 * dy2;
            
            xs_invzs += dszdy20 * dy2;
            xs_invzt += dtzdy20 * dy2;
            
            xe_invzs += dszdy10 * dy2;
            xe_invzt += dtzdy10 * dy2;
        }
        
        // test y1
        if (y1 < kMIN_CLIP_Y) {
            y1 = kMIN_CLIP_Y;
        }
        
        if (y1 > kMAX_CLIP_Y) {
            y1 = kMAX_CLIP_Y;
        }
        
        // test y2
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }
        
        float xs = x0,xe = x0;
        float old_xs = x0,old_xe = x0;
        int endY = y1;
        int start = (int)y0 * mem_pitch;
        
        // 需要用到的变量
        // 需要用到的变量
        float dx;
        float dinvzdx;
        float dinvsdx,dinvtdx;
        float xs_invzx;
        for (int i = y0; i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy10;
                xs = old_xs;
                xe = old_xe;
                
                xs_invz += dinvzdy20;
                xe_invz += dinvzdy10;
                
                xs_invzs += dszdy20;
                xs_invzt += dtzdy20;
                
                xe_invzs += dszdy10;
                xe_invzt += dtzdy10;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            if (distx == 0.0) {
                dinvzdx = 0.0;
                dinvsdx = 0.0;
                dinvtdx = 0.0;
            }else{
                dinvzdx = (xe_invz - xs_invz)/distx;
                dinvsdx = (xe_invzs - xs_invzs)/distx;
                dinvtdx = (xe_invzt - xs_invzt)/distx;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            xs_invzx = xs_invz + dx * dinvzdx;
            
            float begin_invzs = xs_invzs;
            float begin_invzt = xs_invzt;
            for (int k = xs; k <= xe; ++k) {
                float ss = begin_invzs/xs_invzx;
                float st = begin_invzt/xs_invzx;
                
                dst_buffer[start + k] = texmap[clampInt(minTexY,maxTexY,st) * tex_pitch + clampInt(minTexX,maxTexX,ss)];
                
                begin_invzs += dinvsdx;
                begin_invzt += dinvtdx;
                
                xs_invzx += dinvzdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy10;
            xs = old_xs;
            xe = old_xe;
            
            //颜色增量
            xs_invz += dinvzdy20;
            xe_invz += dinvzdy10;
            
            xs_invzs += dszdy20;
            xs_invzt += dtzdy20;
            
            xe_invzs += dszdy10;
            xe_invzt += dtzdy10;
        }
        
        endY = y2;
        xe = x1;
        old_xe = x1;
        
        xe_invz = invz1;
        xe_invzs = invz_s1;
        xe_invzt = invz_t1;
        
        for (int i = y1;i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy21;
                
                xs = old_xs;
                xe = old_xe;
                
                xs_invz += dinvzdy20;
                xe_invz += dinvzdy21;
                
                xs_invzs += dszdy20;
                xs_invzt += dtzdy20;
                
                xe_invzs += dszdy21;
                xe_invzt += dtzdy21;
                
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            if (distx == 0.0) {
                dinvzdx = 0.0;
                dinvsdx = 0.0;
                dinvtdx = 0.0;
            }else{
                dinvzdx = (xe_invz - xs_invz)/distx;
                dinvsdx = (xe_invzs - xs_invzs)/distx;
                dinvtdx = (xe_invzt - xs_invzt)/distx;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            xs_invzx = xs_invz + dx * dinvzdx;
            
            float begin_invzs = xs_invzs;
            float begin_invzt = xs_invzt;
            for (int k = xs; k <= xe; ++k) {
                float ss = begin_invzs/xs_invzx;
                float st = begin_invzt/xs_invzx;
                
                dst_buffer[start + k] = texmap[clampInt(minTexY,maxTexY,st) * tex_pitch + clampInt(minTexX,maxTexX,ss)];
                
                begin_invzs += dinvsdx;
                begin_invzt += dinvtdx;
                
                xs_invzx += dinvzdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy21;
            xs = old_xs;
            xe = old_xe;
            
            // 颜色增量
            xs_invz += dinvzdy20;
            xe_invz += dinvzdy21;
            
            xs_invzs += dszdy20;
            xs_invzt += dtzdy20;
            
            xe_invzs += dszdy21;
            xe_invzt += dtzdy21;
        }
    }
    else{
        printf("DrawTriangle maybe some error......\n");
    }
}