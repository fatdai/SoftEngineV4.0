//
//  CTerrain.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-29.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "CTerrain.h"


bool CTerrain::LoadHeightMap(const char* szFilename,int iSize){
    
    FILE* pFile;
    
    if (m_heightData.m_pucData) {
        UnloadHeightMap();
    }
    
    m_heightData.m_pucData = new unsigned char[iSize * iSize];
    
    //read the heightmap into context
    fread( m_heightData.m_pucData, 1, iSize*iSize, pFile);
    
    // close the file
    fclose(pFile);
    
    // set the size data
    m_heightData.m_iSize = iSize;
    m_iSize = iSize;
    
    return true;
}

bool CTerrain::SaveHeightMap(const char* szFilename ){
    
    FILE* pFile;
    
    pFile = fopen(szFilename,"wb");
    if (pFile == nullptr) {
        printf("Could not create %s \n",szFilename);
        return false;
    }
    
    if (m_heightData.m_pucData == nullptr) {
        printf("高度图数据为空...... %s\n",szFilename);
        return false;
    }
    
    fwrite(m_heightData.m_pucData,1,m_iSize * m_iSize,pFile);
    fclose(pFile);
    
    return true;
}

bool CTerrain::UnloadHeightMap(){
    
    if (m_heightData.m_pucData) {
        // delete the data
        delete [] m_heightData.m_pucData;
        m_heightData.m_iSize = 0;
    }
    
    return true;
}