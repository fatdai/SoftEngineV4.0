//
//  MeshCache.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "MeshCache.h"


MeshCache* MeshCache::_meshCache = nullptr;

MeshCache* MeshCache::getInstance(){
    if (_meshCache == nullptr) {
        _meshCache = new MeshCache;
    }
    return _meshCache;
}


Mesh* MeshCache::addMesh(const string& filename){
    return nullptr;
}


Mesh* MeshCache::getMesh(const string& filename){
    auto it = _cache.begin();
    while (it != _cache.end()) {
        if (it->first == filename) {
            return it->second;
        }
        ++it;
    }
    
    return addMesh(filename);
}


void MeshCache::removeMesh(const string& filename){
    auto it = _cache.begin();
    while (it != _cache.end()) {
        if (it->first == filename) {
            _cache.erase(it);
            return;
        }
        ++it;
    }
}

// 清除所有的
void MeshCache::purge(){
    auto it = _cache.begin();
    while (it != _cache.end()) {
        delete it->second;
        ++it;
    }
    _cache.clear();
}