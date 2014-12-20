//
//  Color.h
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SoftEngineV4_0__Color__
#define __SoftEngineV4_0__Color__

#include <iostream>

class Color{
    
public:
    
    Color(){
        r = g = b = a = 255;
    }
    
    Color(const Color& other){
        color = other.color;
    };
    
    Color(int pr,int pg,int pb){
        r = pr;
        g = pg;
        b = pb;
        a = 255;
    }
    
    ~Color(){};
    
    int toInt_RGB();
    
public:
    
    static int colorToInt_RGB(const Color& color);
    static int colorToInt_RGBA(const Color& color);
    
    static void intToColor_RGB(Color* color,int rgb);
    static void intToColor_RGBA(Color* color,int rgba);
    
public:
    
    union{
        int color;
        
        struct{
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };
    };
};

#endif /* defined(__SoftEngineV4_0__Color__) */
