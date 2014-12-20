//
//  Texture2.cpp
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "Texture2.h"
#include "FileUtil.h"



Texture2* Texture2::createTexture2(const string& filename){
    Texture2* texture = new Texture2;
    
    string fullPath = FileUtil::getInstance()->getFullPath(filename);
    SDL_Surface* loadedSurface = IMG_Load(fullPath.c_str());
    if (loadedSurface == nullptr) {
        printf("could not load image : %s.\n",fullPath.c_str());
        return nullptr;
    }
    
    SDL_Surface* result = SDL_ConvertSurface(loadedSurface, gSurface->format,NULL);
    SDL_FreeSurface(loadedSurface);
    
    texture->surface = result;
    texture->w = result->w;
    texture->h = result->h;
    return texture;
}


