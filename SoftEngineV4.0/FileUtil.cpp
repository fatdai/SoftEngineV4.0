//
//  FileUtil.cpp
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "FileUtil.h"

FileUtil* FileUtil::_fileUtil = nullptr;

string FileUtil::getFullPath(const string& filename){
    return string(_resourcePath + filename);
}


FileUtil* FileUtil::getInstance(){
    if (_fileUtil == nullptr) {
        _fileUtil = new FileUtil;
    }
    return _fileUtil;
}


void FileUtil::setResoursPath(const string& path){
    _resourcePath = path;
}

