//
//  Vertex.h
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#ifndef __SD_LIB__Vertex__
#define __SD_LIB__Vertex__

#include <stdio.h>
#include "ZCMath.h"


//class Vec3{
//public:
//    float x,y,z;
//};
//
//class Vec2{
//public:
//    float x,y;
//};
//
//typedef  Vec3  Point3;
//typedef  Vec2  Point2;

class Vertex{
    
public:
    
    Vertex();
    Vertex(const Vertex& other);
    ~Vertex(){};
    union{
        
        float m[9];
        
        struct{
            Point3 v;
            Vec3 n;
            Point2 t;
        };
        
        struct{
            float x,y,z;
            float nx,ny,nz;
            float tu,tv;
            int color;
        };
    };
};

#endif /* defined(__SD_LIB__Vertex__) */
