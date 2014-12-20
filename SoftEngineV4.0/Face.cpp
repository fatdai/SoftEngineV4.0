//
//  Face.cpp
//  SD_LIB
//
//  Created by dai on 14-12-19.
//  Copyright (c) 2014年 mac. All rights reserved.
//

#include "Face.h"


Face::Face(){
    state = 0;
}

Face::Face(const Face& other){
    
    this->state = other.state;
    
    for (int i = 0; i < 3; ++i) {
   //     vlist_local[i] = other.vlist_local[i];
        vlist_trans[i] = other.vlist_trans[i];
        lit_color[i] = other.lit_color[i];
    }
    this->mati = other.mati;
}