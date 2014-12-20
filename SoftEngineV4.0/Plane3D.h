//
//  ZCPlane3D.h
//  SDL_SoftEngine
//
//  Created by user on 14-11-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SDL_SoftEngine__ZCPlane3D__
#define __SDL_SoftEngine__ZCPlane3D__

#include <iostream>
#include "Vec3.h"

class Plane3D{
   
public:
    
    // 计算点到平面的距离
    // >0 表示在正方向, <0 表示在负方向
    // normal_is_unit 默认为单位向量
    float distance(const Point3& point,bool normal_is_unit = true);
    
public:
    
    // point on plane;
    Point3 p0;
    
    // normal to the plane (not necessarily a unit vector)
    Vec3 n;
    
    

    
};

#endif /* defined(__SDL_SoftEngine__ZCPlane3D__) */
