//
//  Material.h
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#ifndef __SD_LIB__Material__
#define __SD_LIB__Material__

#include <stdio.h>
#include "Texture2.h"

class Material{
    
public:
    
    Material(int type);
    Material();
    
    enum{
        WIRE,  // 线框
        CONSTANT, //  固定着色,不考虑光照处理
        FLAT,  // 恒定着色
        GOURAUD, // gouraud 着色
        TEXTURE, // 纹理
    };
    
    // 材质类型
    int mati_type;
    
    Texture2* texture;
};

#endif /* defined(__SD_LIB__Material__) */
