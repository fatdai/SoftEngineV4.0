//
//  Mesh.h
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#ifndef __SD_LIB__Mesh__
#define __SD_LIB__Mesh__

#include <stdio.h>
#include "FaceIndex.h"

enum{
    MESH_STATE_VISIBLE,
    MESH_STATE_CULLED,
};

class Mesh{
    
public:
    
    // 状态
    int state;
    
    // 面数
    int num_faces;
    
    // 顶点个数
    int num_vertices;
    
    // 面索引数组
    FaceIndex* faceIndexs;
    
    // 顶点列表
    Vertex* vlist;
    
    // 材质
    Material mati;
    
};

#endif /* defined(__SD_LIB__Mesh__) */
