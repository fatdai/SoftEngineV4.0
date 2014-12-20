//
//  TextureCache.cpp
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "TextureCache.h"


TextureCache* TextureCache::_textureCache = nullptr;

TextureCache* TextureCache::getInstance(){
    if (_textureCache == nullptr) {
        _textureCache = new TextureCache;
    }
    return _textureCache;
}

TextureCache::~TextureCache(){
    purge();
}

Texture2* TextureCache::addImage(const string& filename){
    // 先检查是不是在 缓存里面
    auto it = _map.begin();
    while (it != _map.end()) {
        if (it->first == filename) {
            return it->second;
        }
        ++it;
    }
    
    // 不在里面则加入
    Texture2* texture = Texture2::createTexture2(filename);
    _map.insert(pair<string,Texture2*>(filename,texture));
    return texture;
}

Texture2* TextureCache::getTexture(const string& key){
    auto it = _map.begin();
    while (it != _map.end()) {
        if (it->first == key) {
            return it->second;
        }
        ++it;
    }
    printf("could not fin file : %s from cache\n",key.c_str());
    return nullptr;
}

void TextureCache::removeTexture(const string& key){
    auto it = _map.begin();
    while (it != _map.end()) {
        if (it->first == key) {
            _map.erase(it);
            return;
        }
        ++it;
    }
    printf("Could not find key : %s in cache.\n",key.c_str());
}

// 清除所有的
void TextureCache::purge(){
    
    auto it = _map.begin();
    while (it != _map.end()) {
        delete it->second;
        ++it;
    }
    _map.clear();
}
