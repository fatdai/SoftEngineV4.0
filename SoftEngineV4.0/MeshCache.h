//
//  MeshCache.h
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SoftEngineV4_0__MeshCache__
#define __SoftEngineV4_0__MeshCache__

#include <iostream>
#include <map>
#include <string>
#include "Mesh.h"
using namespace std;

class MeshCache{
    
    static MeshCache* _meshCache;
    
public:
    
    static MeshCache* getInstance();
    
    Mesh* addMesh(const string& filename);
    Mesh* getMesh(const string& filename);
    void removeMesh(const string& filename);
    
    // 清除所有的
    void purge();
    
private:
    
    map<string,Mesh*> _cache;
    
};

#endif /* defined(__SoftEngineV4_0__MeshCache__) */
