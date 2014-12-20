//
//  ZCPlane3D.cpp
//  SDL_SoftEngine
//
//  Created by user on 14-11-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "Plane3D.h"


float Plane3D::distance(const Point3& point,bool normal_is_unit)
{
    if (!normal_is_unit) {
        n.normalize();
    }
    return n.dot(point - p0);
}