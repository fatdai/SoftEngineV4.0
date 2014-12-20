//
//  Text2D.h
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SoftEngineV4_0__Text2D__
#define __SoftEngineV4_0__Text2D__

#include <iostream>
#include <string>
#include "Color.h"
using namespace std;


// 屏幕上显示2d文字
class Text2D{

public:
    
    // 默认显示白色
    static void showText(const string& str,int x,int y,const Color& color = Color());
    
};

#endif /* defined(__SoftEngineV4_0__Text2D__) */
