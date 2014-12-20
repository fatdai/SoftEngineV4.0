//
//  Light.h
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SoftEngineV4_0__Light__
#define __SoftEngineV4_0__Light__

#include <iostream>
#include "ZCMath.h"
#include "Global.h"
#include "Color.h"

enum{
    LIGHT_ATTR_AMBIENT = 1, // 环境光
    LIGHT_ATTR_DIRECTIONAL = 1<<1, // 方向光
    LIGHT_ATTR_POINT = 1<<2, // 点光源
    LIGHT_ATTR_SPOTLIGHT1 = 1<<3, // 简单聚灯光
  //  LIGHT_ATTR_SPOTLIGHT2 = 1<<4, // 复杂聚灯光
};


enum{
    LIGHT_STATE_OFF,  // 灯光关
    LIGHT_STATE_ON,
};

class Light{
    
public:
  
    // 创建光源
    Light(){
        state = attr = 0;
        index = -1;
        
        kc = kl = kq = 0;
        spot_inner = spot_outer = 0;
        pf = 0;
    }
    
    // 灯的状态
    int state;
    
    // 灯的属性
    int attr;
    
    // 灯的索引,范围为[0-7]
    int index;
    
    // 环境光 散射光  镜面反射强度
    Color c_ambient;
    Color c_diffuse;
    Color c_specular;
    
    // 光源在世界坐标系的位置和变换后的位置
    Point3 pos,trans_pos;
    
    // 光源在世界坐标系的方向和变换后的方向
    Vec3 dir,trans_dir;
    
    // 衰减系数
    float kc,kl,kq;
    
    // inner angle for spot light
    float spot_inner;
    
    // outer angle for spot light
    float spot_outer;
    
    // power factor/falloff for spot lights
    float pf;
    
};



///////////////////////////////////////////
// 创建一个管理光源的


class LightManager{
    
    static LightManager* _lightManager;
    LightManager();
    
public:
    
    static LightManager* getInstance();
    
    enum{
        MAX_LIGHT_COUNT = 8,
    };
    
    // 创建一个灯光
    // 创建成功返回 灯光的 index
    // 否则返回 -1
    int createLight(int pState,
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
                    );
    
public:
    
    Light mLights[MAX_LIGHT_COUNT];
    
    // 灯数
    int num_light;
    
    
};


#endif /* defined(__SoftEngineV4_0__Light__) */
