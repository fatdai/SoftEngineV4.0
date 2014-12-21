//
//  FPS.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "FPS.h"
#include "Global.h"

const float kTimePerFrame = 1000.0/60.0;

FPS* FPS::_fps = nullptr;

FPS::FPS()
{
    _start = SDL_GetTicks();
    _dt = 0;
    _delay_time = 0;
    
    _fpsRect.x = 0;
    _fpsRect.y = 0;
    
    _renderFaceRect.x = 0;
    _renderFaceRect.y = 30;
    _lastFrameCount = 0;
}

FPS::~FPS(){
    if (_fpsSurface) {
        SDL_FreeSurface(_fpsSurface);
        _fpsSurface = nullptr;
    }
    
    if (_renderFaceSurface) {
        SDL_FreeSurface(_renderFaceSurface);
        _renderFaceSurface = nullptr;
    }
    
    printf(".......~FPS\n");
}

FPS* FPS::getInstance(){
    if (_fps == nullptr) {
        _fps = new FPS;
    }
    return _fps;
}

void FPS::calculateDt(){
    Uint32 curTime = SDL_GetTicks();
    _dt = curTime - _start;
    _start = curTime;
    gRenderFaceCount = 0;
}


void FPS::showFPS(){

    _fpsText.str("");
    _fpsText << "FPS:"<<_lastFrameCount;
    
    _renderFaceText.str("");
    _renderFaceText << "render faces : " << gRenderFaceCount;
    
    if (_fpsSurface) {
        SDL_FreeSurface(_fpsSurface);
        _fpsSurface = nullptr;
    }
    
    if (_renderFaceSurface) {
        SDL_FreeSurface(_renderFaceSurface);
        _renderFaceSurface = nullptr;
    }
    
    _fpsSurface = createTextSurface(_fpsText.str(),Color(0, 255, 0));
    _renderFaceSurface = createTextSurface(_renderFaceText.str(),Color(0, 255, 0));
    
    _fpsRect.w = _fpsSurface->w;
    _fpsRect.h = _fpsSurface->h;
    SDL_BlitSurface(_fpsSurface,NULL,gSurface,&_fpsRect);
    SDL_BlitSurface(_renderFaceSurface,NULL,gSurface,&_renderFaceRect);
}

void FPS::calculateFPS(){
    
    // 计算这一帧使用了多少时间
    Uint32 spent = SDL_GetTicks() - _start;
    if (spent < kTimePerFrame) {
        _delay_time = kTimePerFrame - spent;
        _lastFrameCount = 60.0;
    }else{
        _delay_time = 0;
        _lastFrameCount = 1000.0/(float)spent;
    }
    
    _start = SDL_GetTicks();
    
}

void FPS::release(){
    if (_fpsSurface) {
        SDL_FreeSurface(_fpsSurface);
        _fpsSurface = nullptr;
    }
    
    if (_renderFaceSurface) {
        SDL_FreeSurface(_renderFaceSurface);
        _renderFaceSurface = nullptr;
    }
}

