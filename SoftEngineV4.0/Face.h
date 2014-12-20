//
//  Face.h
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#ifndef __SD_LIB__Face__
#define __SD_LIB__Face__

#include <stdio.h>
#include "Vertex.h"
#include "Material.h"
#include "Color.h"

enum{
    FACE_STATE_ACTIVITY = 1,  //
    FACE_STATE_BACKFACE = 1<<1,
    FACE_STATE_NEED_CUTDOWN = 1<<2,  // 需要进行3D裁减
    FACE_STATE_CLIPPED  = 1<<3,
};

class Face
{
    
public:
    
    Face();
    Face(const Face& other);
    
    // 面状态
    int state;
    
    // 变换前和变换后顶点
 //   Vertex vlist_local[3];
    Vertex vlist_trans[3];
    
    // 面材质
    Material mati;
    
    // 颜色
    Color lit_color[3];
};

#endif /* defined(__SD_LIB__Face__) */
