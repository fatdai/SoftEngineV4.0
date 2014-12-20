//
//  Global.cpp
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "Global.h"

SDL_Surface* gSurface = nullptr;
SDL_Window* gWindow = nullptr;
TTF_Font* gFont = nullptr;

// 显示Text2D文字的Surface
SDL_Surface* gText2DSurface = nullptr;

// 定义几种常见的颜色
Uint32 gColorRed = 0;
Uint32 gColorGreen = 0;
Uint32 gColorBlue = 0;
Uint32 gColorWhite = 0;
Uint32 gColorBlack = 0;
Uint32 gColorYellow = 0;
Uint32 gColorCyan = 0;


// 渲染的面数
int gRenderFaceCount = 0;


//////////////////////////////////////////////////////////////
//// 一些全局方法

SDL_Surface* createTextSurface(const string& text,const Color& color){
    SDL_Surface* loadedSurface = TTF_RenderText_Solid(gFont,text.c_str(),SDL_Color{color.r,color.g,color.b});
    if (loadedSurface == NULL) {
        printf("创建字体失败 Error : %s\n",SDL_GetError());
        return NULL;
    }
    SDL_Surface* result = SDL_ConvertSurface(loadedSurface,gSurface->format,NULL);
    SDL_FreeSurface(loadedSurface);
    return result;
}
