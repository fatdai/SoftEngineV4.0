//
//  Terrain_1.h
//  SoftEngineV4.0
//
//  Created by user on 14-12-31.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SoftEngineV4_0__Terrain_1__
#define __SoftEngineV4_0__Terrain_1__

#include <iostream>
#include <string>
#include "Mesh.h"
#include "Global.h"
#include "ZCMath.h"
using namespace std;

void createTerrain(Mesh* mesh,const string& heightmap,int size,const Point3& scale,const Point3& pos);

#endif /* defined(__SoftEngineV4_0__Terrain_1__) */
