//
//  Global.h
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __Soft_Engine_V3_0__Global__
#define __Soft_Engine_V3_0__Global__

#include <iostream>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "Color.h"
using namespace std;

extern SDL_Surface* gSurface;
extern SDL_Window* gWindow;
extern TTF_Font* gFont;
extern int gRenderFaceCount;

extern SDL_Surface* gText2DSurface;

//屏幕的大小
const static int kWINDOW_WIDTH = 800;
const static int kWINDOW_HEIGHT = 600;

// 常见颜色
extern Uint32 gColorRed;
extern Uint32 gColorGreen;
extern Uint32 gColorBlue;
extern Uint32 gColorWhite;
extern Uint32 gColorBlack;
extern Uint32 gColorYellow;
extern Uint32 gColorCyan;


// bit manipulation macros
// 位操作
#define SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))

// 比较大小
// used to compute the min and max of two expresions
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#ifndef _SD_DEBUG_
#define _SD_DEBUG_
#endif

//////////////////////////////////////////////////////////////
//// 一些全局方法

SDL_Surface* createTextSurface(const string& text,const Color& color);

#endif /* defined(__Soft_Engine_V3_0__Global__) */
