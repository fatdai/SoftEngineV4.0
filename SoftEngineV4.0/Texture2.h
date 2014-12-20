//
//  Texture2.h
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __Soft_Engine_V3_0__Texture2__
#define __Soft_Engine_V3_0__Texture2__

#include <iostream>
#include "Global.h"
#include <string>
using namespace std;



// 不需要自己去管理内存

class Texture2{
    
    static Texture2* createTexture2(const string& filename);
    
public:
    
    friend class TextureCache;
    
    SDL_Surface* surface;
    
    float w,h;
    
};



#endif /* defined(__Soft_Engine_V3_0__Texture2__) */
