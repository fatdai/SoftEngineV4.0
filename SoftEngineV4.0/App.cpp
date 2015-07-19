//
//  App.cpp
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "App.h"
#include "Global.h"
#include "FileUtil.h"
//#include <SDL2_ttf/SDL_ttf.h>
#include "FPS.h"

TTF_Font* loadTTFFont(const string& font,int size){
    return TTF_OpenFont(FileUtil::getInstance()->getFullPath(font).c_str(),size);
}

bool initEngine(const string& resourcesPath){
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error : %s\n",SDL_GetError());
        return false;
    }
    
    //创建窗口
    gWindow = SDL_CreateWindow("SDL Surface",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, kWINDOW_WIDTH, kWINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    
    if (gWindow == NULL) {
        printf("could not create window, SDL_Error : %s\n",SDL_GetError());
        return false;
    }
    
    gSurface = SDL_GetWindowSurface(gWindow);
    
    // 初始化 SDL_image,用来支持 png jpg
    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    bool ok = IMG_Init(imgFlags) & imgFlags;
    if (!ok) {
        printf("SDL_Image could not initialize sdl_image Error : %s \n",IMG_GetError());
        return false;
    }
    
    // 初始化SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize sdl_ttf Error :%s \n",TTF_GetError());
        return false;
    }
    
    FileUtil::getInstance()->setResoursPath(resourcesPath);
    
    //加载字体
    gFont = loadTTFFont("arial.ttf",16);
    if (gFont == NULL) {
        printf("load font bluebold.ttf error.\n");
        return false;
    }
    
    //初始化常见颜色
    gColorRed = SDL_MapRGB(gSurface->format,255, 0, 0);
    gColorGreen = SDL_MapRGB(gSurface->format,0, 255, 0);
    gColorBlue = SDL_MapRGB(gSurface->format,0, 0, 255);
    gColorWhite = SDL_MapRGB(gSurface->format,255, 255, 255);
    gColorBlack = SDL_MapRGB(gSurface->format,0, 0, 0);
    gColorYellow = SDL_MapRGB(gSurface->format,255, 255, 0);
    gColorCyan = SDL_MapRGB(gSurface->format,0, 255, 255);
    
    FPS::getInstance();
    
    return true;
}

void closeEngine(){
    
    // free text2d surface
    if (gText2DSurface) {
        SDL_FreeSurface(gText2DSurface);
        gText2DSurface = nullptr;
    }
    
    //free fps
    FPS::getInstance()->release();
    
    //free font
    TTF_CloseFont(gFont);
    gFont = NULL;
    
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    
    // quit sdl subsystem
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void delayEngine(){
    FPS::getInstance()->calculateFPS();
    SDL_Delay(FPS::getInstance()->getDelayTime());
}





