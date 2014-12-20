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
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

extern SDL_Surface* gSurface;
extern SDL_Window* gWindow;
extern TTF_Font* gFont;


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






#endif /* defined(__Soft_Engine_V3_0__Global__) */
