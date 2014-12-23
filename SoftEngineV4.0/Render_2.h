//
//  Render_2.h
//  SoftEngineV4.0
//
//  Created by user on 14-12-23.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SoftEngineV4_0__Render_2__
#define __SoftEngineV4_0__Render_2__

#include <iostream>
#include "Global.h"
#include "Face.h"
#include "Color.h"

////////////////////////////////////////////////
// 自己来实现光栅化

// 绘制2d直线
//void DrawLine(float x0,float y0,float x1,float y1,Uint32 color);

void DrawLine(int x0,int y0,int x1,int y1,Uint32 color);

// 绘制 Constant Shader 三角形
void DrawTriangle(Face* face);

void DrawTriangle(Vec2* pv);

#endif /* defined(__SoftEngineV4_0__Render_2__) */
