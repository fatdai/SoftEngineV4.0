//
//  RenderTriangle2D.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-24.
//  Copyright (c) 2014年 user. All rights reserved.
//

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

Uint8 clampColorInUint8(float r){
    
    if (r < 0) {
        return 0;
    }
    
    if (r > 255) {
        return 255;
    }
    
    return r;
}

#pragma mark 渲染gouraud 2d三角形
void RenderGouraudTriangle2D(Vec2* pv){
    
    Uint32 color0 = gColorRed;
    Uint32 color1 = gColorGreen;
    Uint32 color2 = gColorBlue;
    
    // first trivial clipping rejection tests
    if (((pv[0].y < kMIN_CLIP_Y)  &&
         (pv[1].y < kMIN_CLIP_Y)  &&
         (pv[2].y < kMIN_CLIP_Y)) ||
        
        ((pv[0].y > kMAX_CLIP_Y)  &&
         (pv[1].y > kMAX_CLIP_Y)  &&
         (pv[2].y > kMAX_CLIP_Y)) ||
        
        ((pv[0].x < kMIN_CLIP_X)  &&
         (pv[1].x < kMIN_CLIP_X)  &&
         (pv[2].x < kMIN_CLIP_X)) ||
        
        ((pv[0].x > kMAX_CLIP_X)  &&
         (pv[1].x > kMAX_CLIP_X)  &&
         (pv[2].x > kMAX_CLIP_X)))
    {
        return;
    }
    
    
    // degenerate triangle
    if ( ((pv[0].x==pv[1].x) &&
          (pv[1].x==pv[2].x)) ||
        ((pv[0].y==pv[1].y) &&
         (pv[1].y==pv[2].y)))
    {
        return;
    }
    
    // 排列顶点
    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp;
    if (pv[v1].y < pv[v0].y) {
        temp = v0;
        v0 = v1;
        v1 = temp;
    }
    
    if (pv[v2].y < pv[v0].y){
        temp = v0;
        v0 = v2;
        v2 = temp;
    }
    
    if (pv[v2].y < pv[v1].y) {
        temp = v1;
        v1 = v2;
        v2 = temp;
    }
    
    TRIANGLE_TYPE tri_type;
    
    // 检测是哪种三角形
    if (pv[v0].y == pv[v1].y) {
        // 平顶三角形
        tri_type = TRIANGLE_TYPE::FLAT_TOP_TRIANGLE;
        
        // 从左到右排列顶点
        if (pv[v1].x < pv[v0].x) {
            temp = v0;
            v0 = v1;
            v1 = temp;
        }
    }//end if
    else{
        if (pv[v1].y == pv[v2].y) {
            // 平底三角形
            tri_type = TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE;
            
            //从左到右
            if (pv[v1].x > pv[v2].x) {
                temp = v1;
                v1 = v2;
                v2 = temp;
            }
        }// end if
        else{
            // 判断三角形是左边长还是右边长
            // 插值计算
            float interx = pv[v0].x + (pv[v1].y - pv[v0].y)*(pv[v2].x - pv[v0].x)/(pv[v2].y - pv[v0].y);
            if (interx > pv[v1].x) {
                tri_type = TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE;
            }else{
                tri_type = TRIANGLE_TYPE::LEFT_LONG_TRIANGLE;
            }
        }// end else
    }// end else
    
    float x0 = pv[v0].x;
    float y0 = pv[v0].y;
    
    float x1 = pv[v1].x;
    float y1 = pv[v1].y;
    
    float x2 = pv[v2].x;
    float y2 = pv[v2].y;
    
    
    Uint32* dst_buffer = (Uint32*)gSurface->pixels;
    int mem_pitch = gSurface->pitch >> 2;
    
    Uint8 r0,g0,b0;
    Uint8 r1,g1,b1;
    Uint8 r2,g2,b2;
    SDL_GetRGB(color0,gSurface->format,&r0,&g0,&b0);
    SDL_GetRGB(color1,gSurface->format,&r1,&g1,&b1);
    SDL_GetRGB(color2,gSurface->format,&r2,&g2,&b2);
    
    // 提取顶点,准备进行光栅化
    if (tri_type == TRIANGLE_TYPE::FLAT_TOP_TRIANGLE) {
        
        float dy = y2 - y0;
        float dxdyl = (x2 - x0)/dy;
        float dxdyr = (x2 - x1)/dy;
        
        // IL
        float drdyl = (r2 - r0)/dy;
        float dgdyl = (g2 - g0)/dy;
        float dbdyl = (b2 - b0)/dy;
        
        // IR
        float drdyr = (r2 - r1)/dy;
        float dgdyr = (g2 - g1)/dy;
        float dbdyr = (b2 - b1)/dy;
        
        float xsr = r0;
        float xsg = g0;
        float xsb = b0;
        
        float xer = r1;
        float xeg = g1;
        float xeb = b1;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            xsr += drdyl * dy2;
            xsg += dgdyl * dy2;
            xsb += dbdyl * dy2;
            
            xer += drdyr * dy2;
            xeg += dgdyr * dy2;
            xeb += dbdyr * dy2;
        }
        
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }
        
        int endY = y2;
        float xs = x0,xe = x1;
        float old_xs = x0,old_xe = x1;
        int start = (int)y0 * mem_pitch;
        
        // 需要用到的变量
        float dx,rs,gs,bs;
        
        for (int i = y0; i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdyl;
                xsg += dgdyl;
                xsb += dbdyl;
                
                xer += drdyr;
                xeg += dgdyr;
                xeb += dbdyr;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            
            float drdx = (xer-xsr)/distx;
            float dgdx = (xeg-xsg)/distx;
            float dbdx = (xeb-xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs), clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
            
            // 颜色增量
            xsr += drdyl;
            xsg += dgdyl;
            xsb += dbdyl;
            
            xer += drdyr;
            xeg += dgdyr;
            xeb += dbdyr;
        }
    }else if(tri_type == TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE){
        
        float dy = y1 - y0;
        float dxdyl = (x1-x0)/dy;
        float dxdyr = (x2-x0)/dy;
        
        //IL
        float drdyl = (r1 - r0)/dy;
        float dgdyl = (g1 - g0)/dy;
        float dbdyl = (b1 - b0)/dy;
        
        //IR
        float drdyr = (r2 - r0)/dy;
        float dgdyr = (g2 - g0)/dy;
        float dbdyr = (b2 - b0)/dy;
        
        float xsr = r0;
        float xsg = g0;
        float xsb = b0;
        
        float xer = r0;
        float xeg = g0;
        float xeb = b0;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            xsr += drdyl * dy2;
            xsg += dgdyl * dy2;
            xsb += dbdyl * dy2;
            
            xer += drdyr * dy2;
            xeg += dgdyr * dy2;
            xeb += dbdyr * dy2;
        }
        
        if (y1 > kMAX_CLIP_Y) {
            y1 = kMAX_CLIP_Y;
        }
        
        float xs = x0,xe = x0;
        float old_xs = x0,old_xe = x0;
        int endY = y1;
        int start = (int)y0 * mem_pitch;
        
        // 需要用到的变量
        float dx,rs,gs,bs;
        
        for (int i = y0; i < endY;++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdyl;
                xsg += dgdyl;
                xsb += dbdyl;
                
                xer += drdyr;
                xeg += dgdyr;
                xeb += dbdyr;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            float drdx = (xer - xsr)/distx;
            float dgdx = (xeg - xsg)/distx;
            float dbdx = (xeb - xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs),clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
            
            // 颜色增量
            xsr += drdyl;
            xsg += dgdyl;
            xsb += dbdyl;
            
            xer += drdyr;
            xeg += dgdyr;
            xeb += dbdyr;
        }
    }
    
    else if (tri_type == TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        //IL1
        float drdy10 = (r1 - r0)/dy10;
        float dgdy10 = (g1 - g0)/dy10;
        float dbdy10 = (b1 - b0)/dy10;
        
        //IR
        float drdy20 = (r2 - r0)/dy20;
        float dgdy20 = (g2 - g0)/dy20;
        float dbdy20 = (b2 - b0)/dy20;
        
        //IL2
        float drdy21 = (r2 - r1)/dy21;
        float dgdy21 = (g2 - g1)/dy21;
        float dbdy21 = (b2 - b1)/dy21;
        
        float xsr = r0;
        float xsg = g0;
        float xsb = b0;
        
        float xer = r0;
        float xeg = g0;
        float xeb = b0;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            xsr += drdy10 * dy2;
            xsg += dgdy10 * dy2;
            xsb += dbdy10 * dy2;
            
            xer += drdy20 * dy2;
            xeg += dgdy20 * dy2;
            xeb += dbdy20 * dy2;
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
        float dx,rs,gs,bs;
        
        for (int i = y0; i < endY; ++i) {
            
            //   assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy10;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdy10;
                xsg += dgdy10;
                xsb += dbdy10;
                
                xer += drdy20;
                xeg += dgdy20;
                xeb += dbdy20;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            float drdx = (xer - xsr)/distx;
            float dgdx = (xeg - xsg)/distx;
            float dbdx = (xeb - xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs),clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy10;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
            
            //颜色增量
            xsr += drdy10;
            xsg += dgdy10;
            xsb += dbdy10;
            
            xer += drdy20;
            xeg += dgdy20;
            xeb += dbdy20;
        }
        
        endY = y2;
        xs = x1;
        old_xs = x1;
        
        xsr = r1;
        xsg = g1;
        xsb = b1;
        
        for (int i = y1; i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy21;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdy21;
                xsg += dgdy21;
                xsb += dbdy21;
                
                xer += drdy20;
                xeg += dgdy20;
                xeb += dbdy20;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            float drdx = (xer - xsr)/distx;
            float dgdx = (xeg - xsg)/distx;
            float dbdx = (xeb - xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs),clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy21;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
            
            // 颜色增量
            xsr += drdy21;
            xsg += dgdy21;
            xsb += dbdy21;
            
            xer += drdy20;
            xeg += dgdy20;
            xeb += dbdy20;
        }
    }else if (tri_type == TRIANGLE_TYPE::LEFT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        // IL
        float drdy20 = (r2 - r0)/dy20;
        float dgdy20 = (g2 - g0)/dy20;
        float dbdy20 = (b2 - b0)/dy20;
        
        //IR1
        float drdy10 = (r1 - r0)/dy10;
        float dgdy10 = (g1 - g0)/dy10;
        float dbdy10 = (b1 - b0)/dy10;
        
        //IR2
        float drdy21 = (r2 - r1)/dy21;
        float dgdy21 = (g2 - g1)/dy21;
        float dbdy21 = (b2 - b1)/dy21;
        
        float xsr = r0;
        float xsg = g0;
        float xsb = b0;
        
        float xer = r0;
        float xeg = g0;
        float xeb = b0;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            xsr += drdy20 * dy2;
            xsg += dgdy20 * dy2;
            xsb += dbdy20 * dy2;
            
            xer += drdy10 * dy2;
            xeg += dgdy10 * dy2;
            xeb += dbdy10 * dy2;
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
        float dx,rs,gs,bs;
        
        for (int i = y0; i < endY; ++i) {
            
            //  assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy10;
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdy20;
                xsg += dgdy20;
                xsb += dbdy20;
                
                xer += drdy10;
                xeg += dgdy10;
                xeb += dbdy10;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            float drdx = (xer - xsr)/distx;
            float dgdx = (xeg - xsg)/distx;
            float dbdx = (xeb - xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs),clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy10;
            xs = old_xs;
            xe = old_xe;
            
            //颜色增量
            xsr += drdy20;
            xsg += dgdy20;
            xsb += dbdy20;
            
            xer += drdy10;
            xeg += dgdy10;
            xeb += dbdy10;
        }
        
        endY = y2;
        xe = x1;
        old_xe = x1;
        
        xer = r1;
        xeg = g1;
        xeb = b1;
        
        for (int i = y1;i < endY; ++i) {
            
            //   assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy21;
                
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdy20;
                xsg += dgdy20;
                xsb += dbdy20;
                
                xer += drdy21;
                xeg += dgdy21;
                xeb += dbdy21;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            float drdx = (xer - xsr)/distx;
            float dgdx = (xeg - xsg)/distx;
            float dbdx = (xeb - xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs),clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy21;
            xs = old_xs;
            xe = old_xe;
            
            // 颜色增量
            xsr += drdy20;
            xsg += dgdy20;
            xsb += dbdy20;
            
            xer += drdy21;
            xeg += dgdy21;
            xeb += dbdy21;
        }
    }
    else{
        printf("DrawTriangle maybe some error......\n");
    }
}


#pragma mark 渲染 gouraud face
void RenderGouraudTriangle2D(Face* face){
        
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
    
    float x1 = face->vlist_trans[v1].x;
    float y1 = face->vlist_trans[v1].y;
    
    float x2 = face->vlist_trans[v2].x;
    float y2 = face->vlist_trans[v2].y;
    
    Uint32* dst_buffer = (Uint32*)gSurface->pixels;
    int mem_pitch = gSurface->pitch >> 2;
    
    unsigned char r0 = face->lit_color[v0].r;
    unsigned char g0 = face->lit_color[v0].g;
    unsigned char b0 = face->lit_color[v0].b;
    
    unsigned char r1 = face->lit_color[v1].r;
    unsigned char g1 = face->lit_color[v1].g;
    unsigned char b1 = face->lit_color[v1].b;
    
    unsigned char r2 = face->lit_color[v2].r;
    unsigned char g2 = face->lit_color[v2].g;
    unsigned char b2 = face->lit_color[v2].b;

//    unsigned char r0 = 255;
//    unsigned char g0 = 0;
//    unsigned char b0 = 0;
//    
//    unsigned char r1 = 0;
//    unsigned char g1 = 255;
//    unsigned char b1 = 0;
//    
//    unsigned char r2 = 0;
//    unsigned char g2 = 0;
//    unsigned char b2 = 255;
    
    // 提取顶点,准备进行光栅化
    if (tri_type == TRIANGLE_TYPE::FLAT_TOP_TRIANGLE) {
        
        float dy = y2 - y0;
        float dxdyl = (x2 - x0)/dy;
        float dxdyr = (x2 - x1)/dy;
        
        // IL
        float drdyl = (r2 - r0)/dy;
        float dgdyl = (g2 - g0)/dy;
        float dbdyl = (b2 - b0)/dy;
        
        // IR
        float drdyr = (r2 - r1)/dy;
        float dgdyr = (g2 - g1)/dy;
        float dbdyr = (b2 - b1)/dy;
        
        float xsr = r0;
        float xsg = g0;
        float xsb = b0;
        
        float xer = r1;
        float xeg = g1;
        float xeb = b1;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            xsr += drdyl * dy2;
            xsg += dgdyl * dy2;
            xsb += dbdyl * dy2;
            
            xer += drdyr * dy2;
            xeg += dgdyr * dy2;
            xeb += dbdyr * dy2;
        }
        
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }
        
        int endY = y2;
        float xs = x0,xe = x1;
        float old_xs = x0,old_xe = x1;
        int start = (int)y0 * mem_pitch;
        
        // 需要用到的变量
        float dx,rs,gs,bs;
        
        for (int i = y0; i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdyl;
                xsg += dgdyl;
                xsb += dbdyl;
                
                xer += drdyr;
                xeg += dgdyr;
                xeb += dbdyr;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            
            float drdx = (xer-xsr)/distx;
            float dgdx = (xeg-xsg)/distx;
            float dbdx = (xeb-xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs), clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
            
            // 颜色增量
            xsr += drdyl;
            xsg += dgdyl;
            xsb += dbdyl;
            
            xer += drdyr;
            xeg += dgdyr;
            xeb += dbdyr;
        }
    }else if(tri_type == TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE){
        
        float dy = y1 - y0;
        float dxdyl = (x1-x0)/dy;
        float dxdyr = (x2-x0)/dy;
        
        //IL
        float drdyl = (r1 - r0)/dy;
        float dgdyl = (g1 - g0)/dy;
        float dbdyl = (b1 - b0)/dy;
        
        //IR
        float drdyr = (r2 - r0)/dy;
        float dgdyr = (g2 - g0)/dy;
        float dbdyr = (b2 - b0)/dy;
        
        float xsr = r0;
        float xsg = g0;
        float xsb = b0;
        
        float xer = r0;
        float xeg = g0;
        float xeb = b0;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            xsr += drdyl * dy2;
            xsg += dgdyl * dy2;
            xsb += dbdyl * dy2;
            
            xer += drdyr * dy2;
            xeg += dgdyr * dy2;
            xeb += dbdyr * dy2;
        }
        
        if (y1 > kMAX_CLIP_Y) {
            y1 = kMAX_CLIP_Y;
        }
        
        float xs = x0,xe = x0;
        float old_xs = x0,old_xe = x0;
        int endY = y1;
        int start = (int)y0 * mem_pitch;
        
        // 需要用到的变量
        float dx,rs,gs,bs;
        
        for (int i = y0; i < endY;++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdyl;
                xsg += dgdyl;
                xsb += dbdyl;
                
                xer += drdyr;
                xeg += dgdyr;
                xeb += dbdyr;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            float drdx = (xer - xsr)/distx;
            float dgdx = (xeg - xsg)/distx;
            float dbdx = (xeb - xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs),clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
            
            // 颜色增量
            xsr += drdyl;
            xsg += dgdyl;
            xsb += dbdyl;
            
            xer += drdyr;
            xeg += dgdyr;
            xeb += dbdyr;
        }
    }
    
    else if (tri_type == TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        //IL1
        float drdy10 = (r1 - r0)/dy10;
        float dgdy10 = (g1 - g0)/dy10;
        float dbdy10 = (b1 - b0)/dy10;
        
        //IR
        float drdy20 = (r2 - r0)/dy20;
        float dgdy20 = (g2 - g0)/dy20;
        float dbdy20 = (b2 - b0)/dy20;
        
        //IL2
        float drdy21 = (r2 - r1)/dy21;
        float dgdy21 = (g2 - g1)/dy21;
        float dbdy21 = (b2 - b1)/dy21;
        
        float xsr = r0;
        float xsg = g0;
        float xsb = b0;
        
        float xer = r0;
        float xeg = g0;
        float xeb = b0;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            xsr += drdy10 * dy2;
            xsg += dgdy10 * dy2;
            xsb += dbdy10 * dy2;
            
            xer += drdy20 * dy2;
            xeg += dgdy20 * dy2;
            xeb += dbdy20 * dy2;
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
        float dx,rs,gs,bs;
        
        for (int i = y0; i < endY; ++i) {
            
            //   assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy10;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdy10;
                xsg += dgdy10;
                xsb += dbdy10;
                
                xer += drdy20;
                xeg += dgdy20;
                xeb += dbdy20;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            float drdx = (xer - xsr)/distx;
            float dgdx = (xeg - xsg)/distx;
            float dbdx = (xeb - xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs),clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy10;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
            
            //颜色增量
            xsr += drdy10;
            xsg += dgdy10;
            xsb += dbdy10;
            
            xer += drdy20;
            xeg += dgdy20;
            xeb += dbdy20;
        }
        
        endY = y2;
        xs = x1;
        old_xs = x1;
        
        xsr = r1;
        xsg = g1;
        xsb = b1;
        
        for (int i = y1; i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy21;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdy21;
                xsg += dgdy21;
                xsb += dbdy21;
                
                xer += drdy20;
                xeg += dgdy20;
                xeb += dbdy20;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            float drdx = (xer - xsr)/distx;
            float dgdx = (xeg - xsg)/distx;
            float dbdx = (xeb - xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs),clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy21;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
            
            // 颜色增量
            xsr += drdy21;
            xsg += dgdy21;
            xsb += dbdy21;
            
            xer += drdy20;
            xeg += dgdy20;
            xeb += dbdy20;
        }
    }else if (tri_type == TRIANGLE_TYPE::LEFT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        // IL
        float drdy20 = (r2 - r0)/dy20;
        float dgdy20 = (g2 - g0)/dy20;
        float dbdy20 = (b2 - b0)/dy20;
        
        //IR1
        float drdy10 = (r1 - r0)/dy10;
        float dgdy10 = (g1 - g0)/dy10;
        float dbdy10 = (b1 - b0)/dy10;
        
        //IR2
        float drdy21 = (r2 - r1)/dy21;
        float dgdy21 = (g2 - g1)/dy21;
        float dbdy21 = (b2 - b1)/dy21;
        
        float xsr = r0;
        float xsg = g0;
        float xsb = b0;
        
        float xer = r0;
        float xeg = g0;
        float xeb = b0;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
            float dy2 = kMIN_CLIP_Y - y0;
            xsr += drdy20 * dy2;
            xsg += dgdy20 * dy2;
            xsb += dbdy20 * dy2;
            
            xer += drdy10 * dy2;
            xeg += dgdy10 * dy2;
            xeb += dbdy10 * dy2;
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
        float dx,rs,gs,bs;
        
        for (int i = y0; i < endY; ++i) {
            
            //  assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy10;
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdy20;
                xsg += dgdy20;
                xsb += dbdy20;
                
                xer += drdy10;
                xeg += dgdy10;
                xeb += dbdy10;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            float drdx = (xer - xsr)/distx;
            float dgdx = (xeg - xsg)/distx;
            float dbdx = (xeb - xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs),clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy10;
            xs = old_xs;
            xe = old_xe;
            
            //颜色增量
            xsr += drdy20;
            xsg += dgdy20;
            xsb += dbdy20;
            
            xer += drdy10;
            xeg += dgdy10;
            xeb += dbdy10;
        }
        
        endY = y2;
        xe = x1;
        old_xe = x1;
        
        xer = r1;
        xeg = g1;
        xeb = b1;
        
        for (int i = y1;i < endY; ++i) {
            
            //   assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy21;
                
                xs = old_xs;
                xe = old_xe;
                
                xsr += drdy20;
                xsg += dgdy20;
                xsb += dbdy20;
                
                xer += drdy21;
                xeg += dgdy21;
                xeb += dbdy21;
                continue;
            }
            
            float distx = old_xe - old_xs;
            
            float drdx = (xer - xsr)/distx;
            float dgdx = (xeg - xsg)/distx;
            float dbdx = (xeb - xsb)/distx;
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            dx = (xs - old_xs);
            rs = xsr + dx * drdx;
            gs = xsg + dx * dgdx;
            bs = xsb + dx * dbdx;
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = SDL_MapRGB(gSurface->format,clampColorInUint8(rs),clampColorInUint8(gs), clampColorInUint8(bs));
                rs += drdx;
                gs += dgdx;
                bs += dbdx;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy21;
            xs = old_xs;
            xe = old_xe;
            
            // 颜色增量
            xsr += drdy20;
            xsg += dgdy20;
            xsb += dbdy20;
            
            xer += drdy21;
            xeg += dgdy21;
            xeb += dbdy21;
        }
    }
    else{
        printf("DrawTriangle maybe some error......\n");
    }
}


#pragma mark 渲染普通2d三角形
void RenderTriangle2D(Vec2* pv){
    
    Uint32 color = gColorRed;
    
    // first trivial clipping rejection tests
    if (((pv[0].y < kMIN_CLIP_Y)  &&
         (pv[1].y < kMIN_CLIP_Y)  &&
         (pv[2].y < kMIN_CLIP_Y)) ||
        
        ((pv[0].y > kMAX_CLIP_Y)  &&
         (pv[1].y > kMAX_CLIP_Y)  &&
         (pv[2].y > kMAX_CLIP_Y)) ||
        
        ((pv[0].x < kMIN_CLIP_X)  &&
         (pv[1].x < kMIN_CLIP_X)  &&
         (pv[2].x < kMIN_CLIP_X)) ||
        
        ((pv[0].x > kMAX_CLIP_X)  &&
         (pv[1].x > kMAX_CLIP_X)  &&
         (pv[2].x > kMAX_CLIP_X)))
    {
        return;
    }
    
    
    // degenerate triangle
    if ( ((pv[0].x==pv[1].x) &&
          (pv[1].x==pv[2].x)) ||
        ((pv[0].y==pv[1].y) &&
         (pv[1].y==pv[2].y)))
    {
        return;
    }
    
    // 排列顶点
    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp;
    if (pv[v1].y < pv[v0].y) {
        temp = v0;
        v0 = v1;
        v1 = temp;
    }
    
    if (pv[v2].y < pv[v0].y){
        temp = v0;
        v0 = v2;
        v2 = temp;
    }
    
    if (pv[v2].y < pv[v1].y) {
        temp = v1;
        v1 = v2;
        v2 = temp;
    }
    
    TRIANGLE_TYPE tri_type;
    
    // 检测是哪种三角形
    if (pv[v0].y == pv[v1].y) {
        // 平顶三角形
        tri_type = TRIANGLE_TYPE::FLAT_TOP_TRIANGLE;
        
        // 从左到右排列顶点
        if (pv[v1].x < pv[v0].x) {
            temp = v0;
            v0 = v1;
            v1 = temp;
        }
    }//end if
    else{
        if (pv[v1].y == pv[v2].y) {
            // 平底三角形
            tri_type = TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE;
            
            //从左到右
            if (pv[v1].x > pv[v2].x) {
                temp = v1;
                v1 = v2;
                v2 = temp;
            }
        }// end if
        else{
            // 判断三角形是左边长还是右边长
            // 插值计算
            float interx = pv[v0].x + (pv[v1].y - pv[v0].y)*(pv[v2].x - pv[v0].x)/(pv[v2].y - pv[v0].y);
            if (interx > pv[v1].x) {
                tri_type = TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE;
            }else{
                tri_type = TRIANGLE_TYPE::LEFT_LONG_TRIANGLE;
            }
        }// end else
    }// end else
    
    float x0 = pv[v0].x;
    float y0 = pv[v0].y;
    
    float x1 = pv[v1].x;
    float y1 = pv[v1].y;
    
    float x2 = pv[v2].x;
    float y2 = pv[v2].y;
    
    
    Uint32* dst_buffer = (Uint32*)gSurface->pixels;
    int mem_pitch = gSurface->pitch >> 2;
    
    // 提取顶点,准备进行光栅化
    if (tri_type == TRIANGLE_TYPE::FLAT_TOP_TRIANGLE) {
        
        float dy = y2 - y0;
        float dxdyl = (x2 - x0)/dy;
        float dxdyr = (x2 - x1)/dy;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
        }
        
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }
        
        int endY = y2;
        float xs = x0,xe = x1;
        float old_xs = x0,old_xe = x1;
        int start = (int)y0 * mem_pitch;
        for (int i = y0; i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
            
        }
    }else if(tri_type == TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE){
        
        
        float dy = y1 - y0;
        float dxdyl = (x1-x0)/dy;
        float dxdyr = (x2-x0)/dy;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
        }
        
        if (y1 > kMAX_CLIP_Y) {
            y1 = kMAX_CLIP_Y;
        }
        
        float xs = x0,xe = x0;
        float old_xs = x0,old_xe = x0;
        int endY = y1;
        int start = (int)y0 * mem_pitch;
        for (int i = y0; i < endY;++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
        }
    }
    
    else if (tri_type == TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
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
        for (int i = y0; i < endY; ++i) {
            
            //   assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy10;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy10;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
        }
        
        endY = y2;
        xs = x1;
        old_xs = x1;
        for (int i = y1; i < endY; ++i) {
            
            //  assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy21;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy21;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
        }
    }else if (tri_type == TRIANGLE_TYPE::LEFT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
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
        for (int i = y0; i < endY; ++i) {
            
            //  assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy10;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy10;
            xs = old_xs;
            xe = old_xe;
        }
        
        endY = y2;
        xe = x1;
        old_xe = x1;
        for (int i = y1;i < endY; ++i) {
            
            //   assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy21;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy21;
            xs = old_xs;
            xe = old_xe;
        }
    }
    else{
        printf("DrawTriangle maybe some error......\n");
    }
}


void RenderTriangle2D(Face* face){
    
    Uint32 color = face->lit_color[0].toInt_RGB();
    
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
    
    // 注意,这里不能换成 int 比较,可能出现问题
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
    // 注意,这里不能换成 int 比较,可能出现问题
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
            // 注意,这里不能换成 int 比较,可能出现问题
            if (face->vlist_trans[v2].x < face->vlist_trans[v1].x){
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
    
    float x1 = face->vlist_trans[v1].x;
    float y1 = face->vlist_trans[v1].y;
    
    float x2 = face->vlist_trans[v2].x;
    float y2 = face->vlist_trans[v2].y;
    
    
    Uint32* dst_buffer = (Uint32*)gSurface->pixels;
    int mem_pitch = gSurface->pitch >> 2;
    
    // 提取顶点,准备进行光栅化
    if (tri_type == TRIANGLE_TYPE::FLAT_TOP_TRIANGLE) {
        
        float dy = y2 - y0;
        float dxdyl = (x2 - x0)/dy;
        float dxdyr = (x2 - x1)/dy;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
        }
        
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }
        
        int endY = y2;
        float xs = x0,xe = x1;
        float old_xs = x0,old_xe = x1;
        int start = (int)y0 * mem_pitch;
        for (int i = y0; i < endY; ++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
            
        }
    }else if(tri_type == TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE){
        
        float dy = y1 - y0;
        float dxdyl = (x1-x0)/dy;
        float dxdyr = (x2-x0)/dy;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
        }
        
        if (y1 > kMAX_CLIP_Y) {
            y1 = kMAX_CLIP_Y;
        }
        
        float xs = x0,xe = x0;
        float old_xs = x0,old_xe = x0;
        int endY = y1;
        int start = (int)y0 * mem_pitch;
        for (int i = y0; i < endY;++i) {
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
        }
    }
    
    else if (tri_type == TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
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
        for (int i = y0; i < endY; ++i) {
            
            //   assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy10;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy10;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
        }
        
        endY = y2;
        xs = x1;
        old_xs = x1;
        for (int i = y1; i < endY; ++i) {
            
            //  assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy21;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy21;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
        }
    }else if (tri_type == TRIANGLE_TYPE::LEFT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
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
        for (int i = y0; i < endY; ++i) {
            
            //  assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy10;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy10;
            xs = old_xs;
            xe = old_xe;
        }
        
        endY = y2;
        xe = x1;
        old_xe = x1;
        for (int i = y1;i < endY; ++i) {
            
            //   assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy21;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy21;
            xs = old_xs;
            xe = old_xe;
        }
    }
    else{
        printf("DrawTriangle maybe some error......\n");
    }
}

