//
//  Material.cpp
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#include "Material.h"


Material::Material(int type){
    mati_type = type;
    ka = kd = ks = 1.0;
    texture = nullptr;
}

Material::Material(){
    mati_type = 0;
    ka = kd = ks = 1.0;
    texture = nullptr;
}