//
//  Vertex.cpp
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#include "Vertex.h"


Vertex::Vertex(){
    memset(m,0,sizeof(float)*kVertexSize);
}


Vertex::Vertex(const Vertex& other){
    for (int i = 0; i < kVertexSize; ++i) {
        m[i] = other.m[i];
    }
}