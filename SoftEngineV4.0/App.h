//
//  App.h
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __Soft_Engine_V3_0__App__
#define __Soft_Engine_V3_0__App__

#include <iostream>
#include <string>
#include "Global.h"
#include "Color.h"
using namespace std;

// 初始化引擎
bool initEngine(const string& resourcesPath);

// 关闭引擎
void closeEngine();

// 加载字体
TTF_Font* loadTTFFont(const string& font,int size);






#endif /* defined(__Soft_Engine_V3_0__App__) */
