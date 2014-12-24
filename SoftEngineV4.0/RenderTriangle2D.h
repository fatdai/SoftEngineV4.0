//
//  RenderTriangle2D.h
//  SoftEngineV4.0
//
//  Created by user on 14-12-24.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SoftEngineV4_0__RenderTriangle2D__
#define __SoftEngineV4_0__RenderTriangle2D__

#include <iostream>
#include "Global.h"
#include "Face.h"
#include "Color.h"

// render 2d 三角形
void RenderTriangle2D(Vec2* pv);

void RenderTriangle2D(Face* face);

// 2d gouraud
void RenderGouraudTriangle2D(Vec2* pv);

void RenderGouraudTriangle2D(Face* face);


// 将r范围限制到[0-255]
Uint8 clampColorInUint8(float r);

#endif /* defined(__SoftEngineV4_0__RenderTriangle2D__) */
