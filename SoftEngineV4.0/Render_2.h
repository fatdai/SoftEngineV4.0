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

#ifndef _DEBUG_RASTER_
#define _DEBUG_RASTER_
#endif


////////////////////////////////////////////////
// 自己来实现光栅化

// 绘制2d直线
//void DrawLine(float x0,float y0,float x1,float y1,Uint32 color);

void DrawLine(int x0,int y0,int x1,int y1,Uint32 color);

// 绘制 Constant Shader 三角形
void DrawTriangle(Face* face);

// 绘制一个 普通的 gouraud shader 三角形
void DrawGouraudTriangle(Face* face);


void DrawGouraudTriangle(Vec2* pv);



#ifdef _DEBUG_RASTER_
// 对于 800 x 600
// 测试三角形坐标  (100.15,84.12) (314.24,84.98) (243.44,233.6)
// 平底三角形坐标  (482.32,224.85) (723.12,224.132) (623.12,23.524)
// 左边长 (23.12,431.12) (296.31,363.12) (333.123,313.45)
//       (403.32,313.12) (598.31,342.11) (732.123,432.11)
// 右边长 (98.11,460.11) (132.12,593.44) (459.11,481.2)
//       (532.11,511.11) (711.11,463.12) (513.18,582.36)

void DrawTriangle(Vec2* pv);

#endif

#endif /* defined(__SoftEngineV4_0__Render_2__) */
