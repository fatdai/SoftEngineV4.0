//
//  ObjLoader.h
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-9.
//  Copyright (c) 2014年 user. All rights reserved.
//

#ifndef __Soft_Engine_V3_0__ObjLoader__
#define __Soft_Engine_V3_0__ObjLoader__

#include <iostream>
#include "ZCMath.h"
#include "Mesh.h"
#include "Texture2.h"
#include <vector>
#include <map>
using namespace std;

class Indicate{
public:
    int x;
    int y;
    int newx;
    int newy;
    
    Indicate(int px,int py,int pnewx,int pnewy):
    x(px),
    y(py),
    newx(pnewx),
    newy(pnewy)
    {
    };
    
    Indicate(int px,int py)
    {
        x = newx = px;
        y = newy = py;
    }
};

class Holder{

public:
    
    void add(int x,int y){
        v.push_back(Indicate(x,y));
    }
    
    void add(int x,int y,int newx,int newy){
        v.push_back(Indicate(x, y, newx, newy));
    }
    
    // 寻找这一对
    int find(int x,int y){
        for (int i = 0; i < v.size(); ++i) {
            if (v[i].x == x && v[i].y == y) {
                return i;
            }
        }
        return -1;
    };
    
    // 是否需要扩展
    bool needExtend(int x,int y){
        for (int i = 0; i < v.size(); ++i) {
            if (v[i].x == x && v[i].y != y) {
                return true;
            }
        }
        return false;
    }
    
    // 处理一组数
    // return true 表示进行了扩展
    // return false 表示没有进行扩展
    bool process(int x,int y,int newX){
        int idx = find(x, y);
        if (idx != -1) {
            // 找到了,不需要进行扩展
            Indicate* indi = &v[idx];
            v.push_back(Indicate(indi->newx,indi->y));
            return false;
        }else{
            // 是否需要扩展
            if (needExtend(x, y)) {
                v.push_back(Indicate(x, y, newX,y));
                return true;
            }else{
                v.push_back(Indicate(x, y));
                return false;
            }
        }
    }
    
    // 计算最大和最小的顶点索引
    void computeMaxVerticesIndex(int* pmin,int* pmax){
        int min = 65535;
        int max = -1;
        
        for (int i = 0; i < v.size(); ++i) {
            if (v[i].newx > max) {
                max = v[i].newx;
            }
            
            if (v[i].newx < min) {
                min = v[i].newx;
            }
        }
        
        if (pmin != nullptr) {
            *pmin = min;
        }
        
        if (pmax != nullptr) {
            *pmax = max;
        }
    }
    
    void print(){
        printf("v.size : %d\n",(int)v.size());
        for (int i = 0; i < v.size()/3; ++i) {
            Indicate* ind0 = &v[3*i];
            Indicate* ind1 = &v[3*i+1];
            Indicate* ind2 = &v[3*i+2];
            printf("%d      %d      %d      %d      \n",ind0->x+1,ind0->y+1,ind0->newx+1,ind0->newy+1);
            printf("%d      %d      %d      %d      \n",ind1->x+1,ind1->y+1,ind1->newx+1,ind1->newy+1);
            printf("%d      %d      %d      %d      \n",ind2->x+1,ind2->y+1,ind2->newx+1,ind2->newy+1);
            printf("-------------------------------------------------------\n");
        }
    }
    
    vector<Indicate> v;
    
};


void vertexnor_to_hmn(map<int,vector<Vec3>* >& hmn,int vertex,const Vec3& vn);

vector<Vec3>* get_vector_from_hmn(map<int,vector<Vec3>* >& hmn,int idx);

void compute_object_radius(Mesh* obj);

#pragma mark 加载OBJ模型相关的方法

// 加载只包含顶点和纹理数据的 obj 模型
// 法线直接通过面数据去计算平均法线
int load_Obj_Vertex_Tex(Mesh* obj,
                        Texture2* texture,
                        const string& filename,
                        const Vec3& scale,
                        const Vec3& pos,
                        bool needNormal = true);



// 加载只包含顶点和法线的 obj 模型
int load_Obj_Vertex_Nor(Mesh* obj,
                        const string& filename,
                        const Vec3& scale,
                        const Vec3& pos
                        );

// 加载只包含顶点的 obj 模型
int load_Obj_Vertex(Mesh* obj,
                    const string& filename,
                    const Vec3& scale,
                    const Vec3& pos,
                    bool needNormal = true);


#endif /* defined(__Soft_Engine_V3_0__ObjLoader__) */
