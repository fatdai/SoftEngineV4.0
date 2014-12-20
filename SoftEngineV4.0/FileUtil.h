//
//  FileUtil.h
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __Soft_Engine_V3_0__FileUtil__
#define __Soft_Engine_V3_0__FileUtil__

#include <iostream>
#include <string>
using namespace std;



    
    class FileUtil{
        
        static FileUtil* _fileUtil;
        
    public:
        
        static FileUtil* getInstance();
        
        string getFullPath(const string& filename);
        
        void setResoursPath(const string& path);
        
    private:
        
        string _resourcePath;
    };


#endif /* defined(__Soft_Engine_V3_0__FileUtil__) */
