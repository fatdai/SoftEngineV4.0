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
using namespace std;

bool initEngine(const string& resourcesPath);
void closeEngine();

// 加载字体
TTF_Font* loadTTFFont(const string& font,int size);

#endif /* defined(__Soft_Engine_V3_0__App__) */
