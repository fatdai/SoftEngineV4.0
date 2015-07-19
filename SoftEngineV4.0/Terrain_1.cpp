//
//  Terrain_1.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-31.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "Terrain_1.h"
#include "FileUtil.h"
#include <vector>
#include "Vertex.h"
#include "FaceIndex.h"
#include "Face.h"
#include "ObjLoader.h"

using namespace std;

void createTerrain(Mesh* mesh,const string& heightmap,int size,const Point3& scale,const Point3& pos){
    
    string fullpath = FileUtil::getInstance()->getFullPath(heightmap);
    
    // 先加载高度图
    FILE* pFile;
    
    unsigned char* buffer = new unsigned char[size * size];
    memset(buffer, 0, sizeof(unsigned char) * size * size);
    
    pFile = fopen(fullpath.c_str(),"rb");
    fread(buffer,1, size * size,pFile);
    fclose(pFile);
    
    SET_BIT(mesh->state,MESH_STATE_VISIBLE);
    
    float heightScale = 0;
    
    // 组织成Mesh
    // 先组织成面索引和定点索引
    vector<Vertex> vertexs;
    for (int z = 0; z <= size; ++z) {
        for (int x = 0; x <= size; ++x) {
            unsigned char height = buffer[z * size + x];
            Vertex v;
            v.attr = VERTEX_ATTR_POINT;
            v.x = x * scale.x;
            v.z = z * scale.z;
            v.y = height * scale.y * heightScale;
            vertexs.push_back(v);
        }
    }
    
    // 面
    vector<int> faceArr;
    for (int z = 0; z < size; ++z) {
        for (int x = 0; x < size; ++x) {
            // 面索引
            faceArr.push_back(z * size + x * size);
            faceArr.push_back(z * size + (x + 1) * size);
            faceArr.push_back((z + 1) * size + (x + 1) * size);
            
            faceArr.push_back(z * size + x * size);
            faceArr.push_back((z + 1) * size + (x + 1) * size);
            faceArr.push_back((z + 1) * size + x * size);
        }
    }

    delete [] buffer;

    // 组织数据
    mesh->num_faces = (int)faceArr.size()/3;
    mesh->world_pos = pos;
    mesh->num_vertices = (int)vertexs.size();
    mesh->initMesh();
    
    // 给顶点赋值
    for (int i = 0; i < mesh->num_faces; ++i) {
        mesh->vlist_local[i] = vertexs[i];
    }
    
    // 给面赋值
    for (int i = 0; i < mesh->num_faces; ++i) {
        
        SET_BIT(mesh->faceIndexs[i].state,FACE_STATE_ACTIVITY);
        mesh->faceIndexs[i].vlist = mesh->vlist_local;
        mesh->faceIndexs[i].vert[0] = faceArr[3*i];
        mesh->faceIndexs[i].vert[1] = faceArr[3*i + 1];
        mesh->faceIndexs[i].vert[2] = faceArr[3*i + 2];
    }
    
    compute_object_radius(mesh);
}