//
//  Light.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "Light.h"





/////////////////////////////////////////////

LightManager* LightManager::_lightManager = nullptr;

LightManager::LightManager(){
    num_light = 0;
}

LightManager* LightManager::getInstance(){
    if (_lightManager == nullptr) {
        _lightManager = new LightManager;
    }
    return _lightManager;
}

int LightManager::createLight(
                   int pState,
                   int pAttr,
                   Color pAmbient,
                   Color pDiffuse,
                   Color pSpecular,
                   const Point3* pPos,
                   const Vec3* pDir,
                   float pkc,
                   float pkl,
                   float pkq,
                   float pSpot_inner,
                   float pSpot_outer,
                   float ppf
                              ){
    
    if (num_light > MAX_LIGHT_COUNT) {
        printf("最多只支持8盏灯......\n");
        return -1;
    }
    
    mLights[num_light].state = pState;
    mLights[num_light].attr = pAttr;
    
    mLights[num_light].c_ambient = pAmbient;
    mLights[num_light].c_diffuse = pDiffuse;
    mLights[num_light].c_specular = pSpecular;
    
    mLights[num_light].kc = pkc;
    mLights[num_light].kl = pkl;
    mLights[num_light].kq = pkq;
    
    mLights[num_light].pf = ppf;
    
    if (pPos) {
        // 点光源
        mLights[num_light].pos = *(pPos);
        mLights[num_light].trans_pos = mLights[num_light].pos;
    }
    
    if (pDir) {
        // 方向光
        mLights[num_light].dir = *(pDir);
        mLights[num_light].dir.normalize();
        mLights[num_light].trans_dir = mLights[num_light].dir;
    }
    
    mLights[num_light].spot_inner = pSpot_inner;
    mLights[num_light].spot_outer = pSpot_outer;
    mLights[num_light].index = num_light;
    ++num_light;
    return (num_light-1);
}