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
    texture = nullptr;
}

Material::Material(){
    mati_type = WIRE;
    texture = nullptr;
}