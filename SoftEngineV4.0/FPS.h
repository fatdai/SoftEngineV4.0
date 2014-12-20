//
//  FPS.h
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SoftEngineV4_0__FPS__
#define __SoftEngineV4_0__FPS__

#include <iostream>
#include "SDL.h"
#include <sstream>
#include <string>
using namespace std;


// 1.计算  FPS
// 2.计算  每帧的 dt
// 3.显示相关信息
class FPS{
    
    static FPS* _fps;
    
    FPS();
    
public:
    
    ~FPS();
    
    static FPS* getInstance();
    
    //  计算 dt
    void calculateDt();
    
    // 显示上一帧FPS
    void showFPS();
    
    // 计算FPS
    void calculateFPS();
    
    float getDt(){
        return _dt;
    }
    
    float getDelayTime(){
        return _delay_time;
    }
    
private:
    
    Uint32 _start;
    
    // fps信息文字
    stringstream _fpsText;
    SDL_Surface* _fpsSurface;
    
    // 渲染的面数文字
    stringstream _renderFaceText;
    SDL_Surface* _renderFaceSurface;
    
    // 每一帧的 dt
    Uint32 _dt;
    
    // 每一帧延迟的时间
    Uint32 _delay_time;
    
    SDL_Rect _fpsRect,_renderFaceRect;
    
    // 上一帧 帧率
    float _lastFrameCount;
};

#endif /* defined(__SoftEngineV4_0__FPS__) */
