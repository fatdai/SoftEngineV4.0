//
//  FaceIndex.h
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#ifndef __SD_LIB__FaceIndex__
#define __SD_LIB__FaceIndex__

#include <stdio.h>
#include "Vertex.h"
#include "Material.h"
#include "Color.h"

class FaceIndex{
    
public:
        
    // 面状态
    int state;
    
    // 顶点列表
    Vertex* vlist;
    
    // 面索引
    int vert[3];
    
    // 面材质
    Material mati;
    
    // 颜色
    Color lit_color[3];
};

#endif /* defined(__SD_LIB__FaceIndex__) */
