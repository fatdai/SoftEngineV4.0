//
//  Text2D.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "Text2D.h"
#include "Global.h"

void Text2D::showText(const string& str,int x,int y,const Color& color){
    if (gText2DSurface) {
        SDL_FreeSurface(gText2DSurface);
        gText2DSurface = nullptr;
    }
    
    gText2DSurface = createTextSurface(str,color);
    SDL_Rect rect = {x,y,gText2DSurface->w,gText2DSurface->h};
    SDL_BlitSurface(gText2DSurface,NULL,gSurface,&rect);
}