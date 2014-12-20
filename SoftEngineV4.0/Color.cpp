//
//  Color.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "Color.h"
#include "Global.h"

int Color::colorToInt_RGB(const Color& color){
    return (int)SDL_MapRGB(gSurface->format,color.r,color.g,color.b);
}

int Color::colorToInt_RGBA(const Color& color){
    return (int)SDL_MapRGBA(gSurface->format,color.r,color.g,color.b,color.a);
}

void Color::intToColor_RGB(Color* color,int rgb){
    SDL_GetRGB(rgb,gSurface->format,&color->r, &color->g, &color->b);
}

void Color::intToColor_RGBA(Color* color,int rgba){
    SDL_GetRGBA(rgba,gSurface->format,&color->r, &color->g, &color->b,&color->a);
}

int Color::toInt_RGB(){
    return SDL_MapRGB(gSurface->format,r,g,b);
}

