//
//  Render.h
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __Soft_Engine_V3_0__Render__
#define __Soft_Engine_V3_0__Render__

#include <iostream>
#include "Global.h"
#include "Face.h"

// defines for texture mapper triangular analysis
#define TRI_TYPE_NONE           0
#define TRI_TYPE_FLAT_TOP       1
#define TRI_TYPE_FLAT_BOTTOM	2
#define TRI_TYPE_FLAT_MASK      3
#define TRI_TYPE_GENERAL        4
#define INTERP_LHS              0
#define INTERP_RHS              1
#define MAX_VERTICES_PER_POLY   6


// used for swapping algorithm
#define SWAP(a,b,t) {t=a; a=b; b=t;}

// 清屏
int renderClear();

// 交换双缓冲
void swapBuffer();


// 进行一些绘制
int draw_line_v1(int x0, int y0, int x1, int y1, Uint32 color);

// 绘制一个实心的矩形
void draw_fill_rect(const SDL_Rect& rect,Uint32 color);

// 绘制一个实心的三角形
void draw_fill_triangle_v1(int x1,int y1,int x2,int y2,int x3,int y3,Uint32 color);

// 绘制一个 gouraud 着色的 三角形
void draw_gouraud_triangle(Face* poly);

// 绘制一个 带texture 的三角形
void draw_texture_triangle(Face* poly);


#endif /* defined(__Soft_Engine_V3_0__Render__) */
