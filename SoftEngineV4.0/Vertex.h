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




enum{
    VERTEX_ATTR_POINT = 1,
    VERTEX_ATTR_NORMAL = 1<<1,
    VERTEX_ATTR_TEXTURE = 1<<2,
};

class Vertex{
    
public:
    
    enum{
        kVertexSize = 9,
    };
    
    Vertex();
    Vertex(const Vertex& other);
    ~Vertex(){};
    
    union{
        
        float m[kVertexSize];
        
        struct{
            Point3 v;
            Vec3 n;
            Point2 t;
        };
        
        struct{
            float x,y,z;
            float nx,ny,nz;
            float tu,tv;
            int attr;
        };
    };
};

#endif /* defined(__SD_LIB__Vertex__) */
