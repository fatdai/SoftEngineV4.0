//
//  TextureCache.h
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __Soft_Engine_V3_0__TextureCache__
#define __Soft_Engine_V3_0__TextureCache__

#include <iostream>
#include <string>
#include <map>
#include "Texture2.h"
using namespace std;


    
    class TextureCache{
    
        static TextureCache* _textureCache;
        
    public:
        
        ~TextureCache();
        
        static TextureCache* getInstance();
        
        Texture2* addImage(const string& filename);
        
        Texture2* getTexture(const string& key);
        
        void removeTexture(const string& key);
        
        // 清除所有的
        void purge();
        
    private:
        
        map<string,Texture2*> _map;
        
    };
    


#endif /* defined(__Soft_Engine_V3_0__TextureCache__) */
