//
//  SDException.cpp
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-17.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "SDException.h"


SDException::SDException(const char* errMsg)
{
    printf("[ZCException] : %s \n",errMsg);
}