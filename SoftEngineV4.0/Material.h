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
        FLAT = 1,
        GOURAUD = 1<<1,
        TEXTURE = 1<<2,
        TEXTURE_FLAT = 1<<3,
        TEXTURE_GOURAUD = 1<<4,
    };
    
    // 光照反射系数
    float ka,kd,ks;
    
    // 材质类型
    int mati_type;
    
    Texture2* texture;
};

#endif /* defined(__SD_LIB__Material__) */
