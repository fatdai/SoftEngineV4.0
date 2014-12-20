//
//  Node.h
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SoftEngineV4_0__Node__
#define __SoftEngineV4_0__Node__

#include <iostream>
#include <vector>
using namespace std;

class Node
{
    
protected:
    
    Node* parent;
    
    vector<Node*> children;
    
};

#endif /* defined(__SoftEngineV4_0__Node__) */
