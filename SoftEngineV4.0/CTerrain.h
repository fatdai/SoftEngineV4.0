//
//  CTerrain.h
//  SoftEngineV4.0
//
//  Created by user on 14-12-29.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __SoftEngineV4_0__CTerrain__
#define __SoftEngineV4_0__CTerrain__

#include <iostream>

struct SHEIGHT_DATA{
    
    unsigned char* m_pucData; // height data
    
    int m_iSize; // the height size(power of 2)
};


class CTerrain{
    
protected:
    
    SHEIGHT_DATA m_heightData; // the height data
    
    float m_fHeightScale; // scaling variable
    
public:
    
    int m_iSize; // must be a powe of 2
    
    CTerrain(){
        m_fHeightScale = 1.0;
    };
    ~CTerrain(){};
    
public:
    

    virtual void Render() = 0;
    
    bool LoadHeightMap(const char* szFilename,int iSize);
    
    bool SaveHeightMap(const char* szFilename );
    
    bool UnloadHeightMap();
    
    void SetHeightScale( float fScale ) {
        m_fHeightScale= fScale;
    }
    
    void SetHeightAtPoint( unsigned char ucHeight,int iX, int iZ){
        m_heightData.m_pucData[( iZ*m_iSize )+iX]= ucHeight;
    }
    
    unsigned char GetTrueHeightAtPoint(int iX,int iZ){
        return m_heightData.m_pucData[iZ * m_iSize + iX];
    }
    
    float GetScaledHeightAtPoint(int iX,int iZ){
        return m_heightData.m_pucData[iZ * m_iSize + iX] * m_fHeightScale;
    }
    

    
};


#endif /* defined(__SoftEngineV4_0__CTerrain__) */
