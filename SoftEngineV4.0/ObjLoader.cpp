//
//  ObjLoader.cpp
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-9.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "ObjLoader.h"
#include "FileUtil.h"
#include "FaceIndex.h"
#include "Face.h"
#include "SDException.h"
#include <vector>
#include "Global.h"
#include <assert.h>
using namespace std;


typedef struct{
    int vert[3];
    int nor[3];
    int tex[3];
}FaceIndexHolder;

void compute_object_radius(Mesh* obj)
{
    
    obj->avg_radius = 0;
    obj->max_radius = 0;
    
    for (int vertex = 0; vertex < obj->num_vertices; ++vertex) {
        float distance = sqrtf(obj->vlist_local[vertex].x * obj->vlist_local[vertex].x + obj->vlist_local[vertex].y * obj->vlist_local[vertex].y + obj->vlist_local[vertex].z * obj->vlist_local[vertex].z);
        
        obj->avg_radius += distance;
        
        if (distance > obj->max_radius) {
            obj->max_radius = distance;
        }
    }
    
    obj->avg_radius /= obj->num_vertices;
}

#pragma mark 加载只包含顶点和纹理数据的 obj 模型

int load_Obj_Vertex_Tex(Mesh* obj,
             Texture2* texture,
             const string& filename,
             const Vec3& scale,
             const Vec3& pos,
             bool needNormal
             ){
    
    char cLine[256];	//A line of the obj file
    vector<float> vertices;
    vector<float> texCoords;
    vector<float> normals;
    vector<FaceIndexHolder> polys;
    bool hasTexture = false;
    bool hasNormal = false;
    
    //设置成激活 和 可见状态
    SET_BIT(obj->state,MESH_STATE_VISIBLE);
    
    // 设置位置
    obj->world_pos = pos;
    
    string fullpath = FileUtil::getInstance()->getFullPath(filename);
    FILE* fp = fopen(fullpath.c_str(),"rt");
    if (!fp) {
        printf("could not open %s \n",filename.c_str());
        return (0);
    }
    
    while (!feof(fp)) {
        //Check the first char in the line
		int iStart = fgetc(fp);
        
		//If the first letter is v, it is either a vertex, a text coord, or a vertex normal
		if(iStart == 'v')
		{
			//get the second char
			int iNext = fgetc(fp);
			float fTemp[3];
            
			//if its a space, its a vertex coordinate
			if(iNext == ' ' || iNext == '\t')
			{
				//get the line
				fgets(cLine, 256, fp);
				//get the vertex coords
				sscanf(cLine, " %f %f %f", &fTemp[0], &fTemp[1], &fTemp[2]);
				//add to the vertex array
				vertices.push_back(fTemp[0]);
                vertices.push_back(fTemp[1]);
                vertices.push_back(fTemp[2]);
			}
			//if its a t, its a texture coord
			else if(iNext == 't')
			{
				//get the line
				fgets(cLine, 256, fp);
				//get the vertex coords
				sscanf(cLine, " %f %f", &fTemp[0], &fTemp[1]);
				//add to the vertex array
				texCoords.push_back(fTemp[0]);
                texCoords.push_back(fTemp[1]);
				hasTexture = true;
			}
			//if its an n its a normal
			else if(iNext == 'n')
			{
				//get the line
				fgets(cLine, 256, fp);
				//get the vertex coords
				sscanf(cLine, " %f %f %f", &fTemp[0], &fTemp[1], &fTemp[2]);
				//add to the vertex array
				normals.push_back(fTemp[0]);
                normals.push_back(fTemp[1]);
                normals.push_back(fTemp[2]);
				hasNormal = true;
			}
			//else its something we don't support
			else
			{
				//scan the line and discard it
				fgets(cLine, 256, fp);
			}
		}
		//if the first letter is f, its a face
		else if(iStart == 'f')
		{
			//temp buffer to hold vertex indices
            //			int iTemp[3][3];
            FaceIndexHolder poly;
         //   poly.state = POLY_STATE_ACTIVE;
            
			//read in the line
			fgets(cLine, 256, fp);
            
			//If it has texture coords AND vertex normals
			if(hasTexture && hasNormal)
			{
				//extract the face info
				sscanf(cLine, " %i/%i/%i %i/%i/%i %i/%i/%i", &poly.vert[0], &poly.tex[0], &poly.nor[0],
                       &poly.vert[1], &poly.tex[1], &poly.nor[1],
                       &poly.vert[2], &poly.tex[2], &poly.nor[2]);
                
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                    poly.tex[k] -= 1;
                    poly.nor[k] -= 1;
                }
                
				//store the info in the faces structure
				polys.push_back(poly);
				
			}
			//Just has tex coords
			else if(hasTexture && !hasNormal)
			{
				//extract the face info
				sscanf(cLine, " %i/%i %i/%i %i/%i", &poly.vert[0], &poly.tex[0],
                       &poly.vert[1], &poly.tex[1],
                       &poly.vert[2], &poly.tex[2]);
				
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                    poly.tex[k] -= 1;
                }
                
				//store the info in the faces structure
				polys.push_back(poly);
			}
			//just normals
			else if(!hasTexture && hasNormal)
			{
				sscanf(cLine, " %i//%i %i//%i %i//%i", &poly.vert[0], &poly.nor[0],
                       &poly.vert[1], &poly.nor[1],
                       &poly.vert[2], &poly.nor[2]);
                
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                    poly.nor[k] -= 1;
                }
                
				//store the info in the faces structure
				polys.push_back(poly);
                
			}
			//Just vertices
			else
			{
				//extract the face info
				sscanf(cLine, "%i %i %i", &poly.vert[0], &poly.vert[1], &poly.vert[2]);
				//store the info in the faces structure
                
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                }
                
				polys.push_back(poly);
			}
			
		}
		//if it isn't any of those, we don't care about it
		else
		{
			//read the whole line to advance
			fgets(cLine, 256, fp);
		}
    }
    
    fclose(fp);
    
    
    if (!hasTexture) {
        throw new SDException("此obj文件不包含纹理数据............\n");
    }
    
    SET_BIT(obj->mati.mati_type,Material::TEXTURE);
    obj->mati.texture = texture;

    /////////////////////////////////
    // 一次处理所有面索引
    Holder holder;
    int count = (int)vertices.size()/3;
    for (int i = 0 ; i < polys.size(); ++i) {
        FaceIndexHolder* poly_index = &polys[i];
        int vertex0 = poly_index->vert[0];
        int vertex1 = poly_index->vert[1];
        int vertex2 = poly_index->vert[2];
        
        int tex0 = poly_index->tex[0];
        int tex1 = poly_index->tex[1];
        int tex2 = poly_index->tex[2];
        
        if (holder.process(vertex0,tex0,count)) {
            vertices.push_back(vertices[3*vertex0]);
            vertices.push_back(vertices[3*vertex0+1]);
            vertices.push_back(vertices[3*vertex0+2]);
            poly_index->vert[0] = count;
            ++count;
        }
        
        if (holder.process(vertex1,tex1,count)) {
            vertices.push_back(vertices[3*vertex1]);
            vertices.push_back(vertices[3*vertex1+1]);
            vertices.push_back(vertices[3*vertex1+2]);
            poly_index->vert[1] = count;
            ++count;
        }

        if (holder.process(vertex2,tex2,count)) {
            vertices.push_back(vertices[3*vertex2]);
            vertices.push_back(vertices[3*vertex2+1]);
            vertices.push_back(vertices[3*vertex2+2]);
            poly_index->vert[2] = count;
            ++count;
        }
    }
    //////////////////// 处理后的索引/////////////

// for debug
//    holder.print();
    
    // blender 里面导出时,纹理坐标是从左下脚开始算的
    for (int i = 0; i < texCoords.size()/2; ++i) {
        texCoords[2*i] *= texture->w;
        texCoords[2*i + 1] = (1.0 - texCoords[2*i + 1]) * texture->h;
    }
    
    //
    obj->num_faces = (int)polys.size();
    int min,max;
    holder.computeMaxVerticesIndex(&min,&max);
    obj->num_vertices = max+1;
    
    // 分配内存
    obj->initMesh();
    
    // 平均前,每个顶点对应的法向量的集合
    map<int,vector<Vec3>* > hmn;
    
    assert((holder.v.size()/3) == polys.size());
    for (int i = 0; i < polys.size(); ++i) {
        
        int vertex0 = holder.v[3*i].newx;
        int vertex1 = holder.v[3*i+1].newx;
        int vertex2 = holder.v[3*i+2].newx;
        
        int tex0 = holder.v[3*i].newy;
        int tex1 = holder.v[3*i+1].newy;
        int tex2 = holder.v[3*i+2].newy;
        
        Vec3 v0(vertices[3*vertex0],vertices[3*vertex0+1],vertices[3*vertex0+2]);
        Vec3 v1(vertices[3*vertex1],vertices[3*vertex1+1],vertices[3*vertex1+2]);
        Vec3 v2(vertices[3*vertex2],vertices[3*vertex2+1],vertices[3*vertex2+2]);
        
        obj->vlist_local[vertex0].x = v0.x * scale.x;
        obj->vlist_local[vertex0].y = v0.y * scale.y;
        obj->vlist_local[vertex0].z = v0.z * scale.z;
        
        obj->vlist_local[vertex1].x = v1.x * scale.x;
        obj->vlist_local[vertex1].y = v1.y * scale.y;
        obj->vlist_local[vertex1].z = v1.z * scale.z;
        
        obj->vlist_local[vertex2].x = v2.x * scale.x;
        obj->vlist_local[vertex2].y = v2.y * scale.y;
        obj->vlist_local[vertex2].z = v2.z * scale.z;
        
        
        obj->vlist_local[vertex0].tu = texCoords[2*tex0];
        obj->vlist_local[vertex0].tv = texCoords[2*tex0+1];
        
        obj->vlist_local[vertex1].tu = texCoords[2*tex1];
        obj->vlist_local[vertex1].tv = texCoords[2*tex1+1];
        
        obj->vlist_local[vertex2].tu = texCoords[2*tex2];
        obj->vlist_local[vertex2].tv = texCoords[2*tex2+1];
        
        if (needNormal) {
            Vec3 v01 = v1 - v0;
            Vec3 v02 = v2 - v0;
            Vec3 vn;
            Vec3::cross(v01,v02,&vn);
            
            vertexnor_to_hmn(hmn,vertex0,vn);
            vertexnor_to_hmn(hmn,vertex1,vn);
            vertexnor_to_hmn(hmn,vertex2,vn);
        }
        
        obj->faceIndexs[i].state = FACE_STATE_ACTIVITY;
        obj->faceIndexs[i].vlist = obj->vlist_local;
        obj->faceIndexs[i].mati = obj->mati;
        
        // 最新的索引
        obj->faceIndexs[i].vert[0] = vertex0;
        obj->faceIndexs[i].vert[1] = vertex1;
        obj->faceIndexs[i].vert[2] = vertex2;
    }
    
    compute_object_radius(obj);
    
    if (needNormal) {
        for (int i = 0; i < obj->num_vertices; ++i) {
            vector<Vec3>* tmpv = get_vector_from_hmn(hmn,i);
            if (nullptr != tmpv) {
                Vec3 tnv;
                for (int k = 0; k < tmpv->size(); ++k) {
                    tnv += (*tmpv)[k];
                }
                tnv.normalize();
                obj->vlist_local[i].n = tnv;
            }else{
                printf("顶点:%d   无法线.......\n",i);
            }
        }
        
        // 释放内存
        auto it = hmn.begin();
        while (it != hmn.end()) {
            delete it->second;
            ++it;
        }
        hmn.clear();
    }

    return (1);
}


#pragma mark 加载只包含顶点和法线的 obj 模型
int load_Obj_Vertex_Nor(Mesh* obj,
                        const string& filename,
                        const Vec3& scale,
                        const Vec3& pos
                        )
{
    char cLine[256];	//A line of the obj file
    vector<float> vertices;
    vector<float> texCoords;
    vector<float> normals;
    vector<FaceIndexHolder> polys;
    bool hasTexture = false;
    bool hasNormal = false;
    
    //设置成激活 和 可见状态
    SET_BIT(obj->state, MESH_STATE_VISIBLE);
    
    // 设置位置
    obj->world_pos = pos;
    
    string fullpath = FileUtil::getInstance()->getFullPath(filename);
    FILE* fp = fopen(fullpath.c_str(),"rt");
    if (!fp) {
        printf("could not open %s \n",filename.c_str());
        return (0);
    }
    
    while (!feof(fp)) {
        //Check the first char in the line
		int iStart = fgetc(fp);
        
		//If the first letter is v, it is either a vertex, a text coord, or a vertex normal
		if(iStart == 'v')
		{
			//get the second char
			int iNext = fgetc(fp);
			float fTemp[3];
            
			//if its a space, its a vertex coordinate
			if(iNext == ' ' || iNext == '\t')
			{
				//get the line
				fgets(cLine, 256, fp);
				//get the vertex coords
				sscanf(cLine, " %f %f %f", &fTemp[0], &fTemp[1], &fTemp[2]);
				//add to the vertex array
				vertices.push_back(fTemp[0]);
                vertices.push_back(fTemp[1]);
                vertices.push_back(fTemp[2]);
			}
			//if its a t, its a texture coord
			else if(iNext == 't')
			{
				//get the line
				fgets(cLine, 256, fp);
				//get the vertex coords
				sscanf(cLine, " %f %f", &fTemp[0], &fTemp[1]);
				//add to the vertex array
				texCoords.push_back(fTemp[0]);
                texCoords.push_back(fTemp[1]);
				hasTexture = true;
			}
			//if its an n its a normal
			else if(iNext == 'n')
			{
				//get the line
				fgets(cLine, 256, fp);
				//get the vertex coords
				sscanf(cLine, " %f %f %f", &fTemp[0], &fTemp[1], &fTemp[2]);
				//add to the vertex array
				normals.push_back(fTemp[0]);
                normals.push_back(fTemp[1]);
                normals.push_back(fTemp[2]);
				hasNormal = true;
			}
			//else its something we don't support
			else
			{
				//scan the line and discard it
				fgets(cLine, 256, fp);
			}
		}
		//if the first letter is f, its a face
		else if(iStart == 'f')
		{
			//temp buffer to hold vertex indices
            //			int iTemp[3][3];
            FaceIndexHolder poly;
            
			//read in the line
			fgets(cLine, 256, fp);
            
			//If it has texture coords AND vertex normals
			if(hasTexture && hasNormal)
			{
				//extract the face info
				sscanf(cLine, " %i/%i/%i %i/%i/%i %i/%i/%i", &poly.vert[0], &poly.tex[0], &poly.nor[0],
                       &poly.vert[1], &poly.tex[1], &poly.nor[1],
                       &poly.vert[2], &poly.tex[2], &poly.nor[2]);
                
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                    poly.tex[k] -= 1;
                    poly.nor[k] -= 1;
                }
                
				//store the info in the faces structure
				polys.push_back(poly);
				
			}
			//Just has tex coords
			else if(hasTexture && !hasNormal)
			{
				//extract the face info
				sscanf(cLine, " %i/%i %i/%i %i/%i", &poly.vert[0], &poly.tex[0],
                       &poly.vert[1], &poly.tex[1],
                       &poly.vert[2], &poly.tex[2]);
				
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                    poly.tex[k] -= 1;
                }
                
				//store the info in the faces structure
				polys.push_back(poly);
			}
			//just normals
			else if(!hasTexture && hasNormal)
			{
				sscanf(cLine, " %i//%i %i//%i %i//%i", &poly.vert[0], &poly.nor[0],
                       &poly.vert[1], &poly.nor[1],
                       &poly.vert[2], &poly.nor[2]);
                
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                    poly.nor[k] -= 1;
                }
                
				//store the info in the faces structure
				polys.push_back(poly);
                
			}
			//Just vertices
			else
			{
				//extract the face info
				sscanf(cLine, "%i %i %i", &poly.vert[0], &poly.vert[1], &poly.vert[2]);
				//store the info in the faces structure
                
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                }
                
				polys.push_back(poly);
			}
			
		}
		//if it isn't any of those, we don't care about it
		else
		{
			//read the whole line to advance
			fgets(cLine, 256, fp);
		}
    }
    
    fclose(fp);
    
    if (!hasNormal) {
        throw new SDException("此模型不包含法线数据.\n");
    }
    
    // 由于不包括纹理,所以狠简单的处理
    obj->num_faces = (int)polys.size();
    obj->num_vertices = (int)vertices.size()/3;
    
    // 分配内存
    obj->initMesh();
    
    for (int i = 0; i < polys.size(); ++i) {
        
        FaceIndexHolder* ploy_index = &polys[i];
        int vertex0 = ploy_index->vert[0];
        int vertex1 = ploy_index->vert[1];
        int vertex2 = ploy_index->vert[2];
        
        int nor0 = ploy_index->nor[0];
        int nor1 = ploy_index->nor[1];
        int nor2 = ploy_index->nor[2];
        
        obj->vlist_local[vertex0].x = vertices[3*vertex0] * scale.x;
        obj->vlist_local[vertex0].y = vertices[3*vertex0+1]* scale.y;
        obj->vlist_local[vertex0].z = vertices[3*vertex0+2]* scale.z;
        
        obj->vlist_local[vertex1].x = vertices[3*vertex1]* scale.x;
        obj->vlist_local[vertex1].y = vertices[3*vertex1+1]* scale.y;
        obj->vlist_local[vertex1].z = vertices[3*vertex1+2]* scale.z;
        
        obj->vlist_local[vertex2].x = vertices[3*vertex2]* scale.x;
        obj->vlist_local[vertex2].y = vertices[3*vertex2+1]* scale.y;
        obj->vlist_local[vertex2].z = vertices[3*vertex2+2]* scale.z;
        
        obj->vlist_local[vertex0].nx = normals[3*nor0];
        obj->vlist_local[vertex0].ny = texCoords[3*nor0+1];
        obj->vlist_local[vertex0].nz = texCoords[3*nor0+2];
        
        obj->vlist_local[vertex1].nx = normals[3*nor1];
        obj->vlist_local[vertex1].ny = texCoords[3*nor1+1];
        obj->vlist_local[vertex1].nz = texCoords[3*nor1+2];
        
        obj->vlist_local[vertex2].nx = normals[3*nor2];
        obj->vlist_local[vertex2].ny = texCoords[3*nor2+1];
        obj->vlist_local[vertex2].nz = texCoords[3*nor2+2];
        
        ///////////
        // 给 faceIndexs 赋值
        obj->faceIndexs[i].state = FACE_STATE_ACTIVITY;
        obj->faceIndexs[i].vlist = obj->vlist_local;
        obj->faceIndexs[i].mati = obj->mati;
        
        // 最新的索引
        obj->faceIndexs[i].vert[0] = vertex0;
        obj->faceIndexs[i].vert[1] = vertex1;
        obj->faceIndexs[i].vert[2] = vertex2;
    }
    
    compute_object_radius(obj);
    
    return (1);
}



// 查找顶点 vertex 在 hmn 里面是否存在
void vertexnor_to_hmn(map<int,vector<Vec3>* >& hmn,int vertex,const Vec3& vn)
{
    
    vector<Vec3>* tmpv = nullptr;
    auto it = hmn.begin();
    bool found = false;
    while (it != hmn.end()) {
        if (it->first == vertex) {
            found = true;
            tmpv = it->second;
            break;
        }
        ++it;
    }
    
    if (found) {
        tmpv->push_back(vn);
    }else{
        tmpv = new vector<Vec3>;
        tmpv->push_back(vn);
        hmn.insert(pair<int,vector<Vec3>* >(vertex,tmpv));
    }
}

vector<Vec3>* get_vector_from_hmn(map<int,vector<Vec3>* >& hmn,int idx){
    auto it = hmn.begin();
    while (it != hmn.end()) {
        if (it->first == idx) {
            return it->second;
        }
        ++it;
    }
    return nullptr;
}

#pragma mark 加载只包含顶点的 obj 模型
int load_Obj_Vertex(Mesh* obj,
                    const string& filename,
                    const Vec3& scale,
                    const Vec3& pos,
                    bool needNormal){
    
    char cLine[256];	//A line of the obj file
    vector<float> vertices;
    vector<float> texCoords;
    vector<float> normals;
    vector<FaceIndexHolder> polys;
    bool hasTexture = false;
    bool hasNormal = false;
    
    //设置成激活 和 可见状态
    SET_BIT(obj->state, MESH_STATE_VISIBLE);
    
    // 设置位置
    obj->world_pos = pos;
    
    string fullpath = FileUtil::getInstance()->getFullPath(filename);
    FILE* fp = fopen(fullpath.c_str(),"rt");
    if (!fp) {
        printf("could not open %s \n",filename.c_str());
        return (0);
    }
    
    while (!feof(fp)) {
        //Check the first char in the line
		int iStart = fgetc(fp);
        
		//If the first letter is v, it is either a vertex, a text coord, or a vertex normal
		if(iStart == 'v')
		{
			//get the second char
			int iNext = fgetc(fp);
			float fTemp[3];
            
			//if its a space, its a vertex coordinate
			if(iNext == ' ' || iNext == '\t')
			{
				//get the line
				fgets(cLine, 256, fp);
				//get the vertex coords
				sscanf(cLine, " %f %f %f", &fTemp[0], &fTemp[1], &fTemp[2]);
				//add to the vertex array
				vertices.push_back(fTemp[0]);
                vertices.push_back(fTemp[1]);
                vertices.push_back(fTemp[2]);
			}
			//if its a t, its a texture coord
			else if(iNext == 't')
			{
				//get the line
				fgets(cLine, 256, fp);
				//get the vertex coords
				sscanf(cLine, " %f %f", &fTemp[0], &fTemp[1]);
				//add to the vertex array
				texCoords.push_back(fTemp[0]);
                texCoords.push_back(fTemp[1]);
				hasTexture = true;
			}
			//if its an n its a normal
			else if(iNext == 'n')
			{
				//get the line
				fgets(cLine, 256, fp);
				//get the vertex coords
				sscanf(cLine, " %f %f %f", &fTemp[0], &fTemp[1], &fTemp[2]);
				//add to the vertex array
				normals.push_back(fTemp[0]);
                normals.push_back(fTemp[1]);
                normals.push_back(fTemp[2]);
				hasNormal = true;
			}
			//else its something we don't support
			else
			{
				//scan the line and discard it
				fgets(cLine, 256, fp);
			}
		}
		//if the first letter is f, its a face
		else if(iStart == 'f')
		{
			//temp buffer to hold vertex indices
            //			int iTemp[3][3];
            FaceIndexHolder poly;
            
			//read in the line
			fgets(cLine, 256, fp);
            
			//If it has texture coords AND vertex normals
			if(hasTexture && hasNormal)
			{
				//extract the face info
				sscanf(cLine, " %i/%i/%i %i/%i/%i %i/%i/%i", &poly.vert[0], &poly.tex[0], &poly.nor[0],
                       &poly.vert[1], &poly.tex[1], &poly.nor[1],
                       &poly.vert[2], &poly.tex[2], &poly.nor[2]);
                
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                    poly.tex[k] -= 1;
                    poly.nor[k] -= 1;
                }
                
				//store the info in the faces structure
				polys.push_back(poly);
				
			}
			//Just has tex coords
			else if(hasTexture && !hasNormal)
			{
				//extract the face info
				sscanf(cLine, " %i/%i %i/%i %i/%i", &poly.vert[0], &poly.tex[0],
                       &poly.vert[1], &poly.tex[1],
                       &poly.vert[2], &poly.tex[2]);
				
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                    poly.tex[k] -= 1;
                }
                
				//store the info in the faces structure
				polys.push_back(poly);
			}
			//just normals
			else if(!hasTexture && hasNormal)
			{
				sscanf(cLine, " %i//%i %i//%i %i//%i", &poly.vert[0], &poly.nor[0],
                       &poly.vert[1], &poly.nor[1],
                       &poly.vert[2], &poly.nor[2]);
                
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                    poly.nor[k] -= 1;
                }
                
				//store the info in the faces structure
				polys.push_back(poly);
                
			}
			//Just vertices
			else
			{
				//extract the face info
				sscanf(cLine, "%i %i %i", &poly.vert[0], &poly.vert[1], &poly.vert[2]);
				//store the info in the faces structure
                
                for (int k = 0; k < 3; ++k) {
                    poly.vert[k] -= 1;
                }
                
				polys.push_back(poly);
			}
			
		}
		//if it isn't any of those, we don't care about it
		else
		{
			//read the whole line to advance
			fgets(cLine, 256, fp);
		}
    }
    
    fclose(fp);
    
    // 由于不包括纹理,所以狠简单的处理
    obj->num_faces = (int)polys.size();
    obj->num_vertices = (int)vertices.size()/3;
    
    // 分配内存
    obj->initMesh();
    
    // 平均前,每个顶点对应的法向量的集合
    map<int,vector<Vec3>* > hmn;
    
    for (int i = 0; i < polys.size(); ++i) {
        
        FaceIndexHolder* ploy_index = &polys[i];
        int vertex0 = ploy_index->vert[0];
        int vertex1 = ploy_index->vert[1];
        int vertex2 = ploy_index->vert[2];
        
        // 计算法向量
        Vec3 v0(vertices[3*vertex0],vertices[3*vertex0+1],vertices[3*vertex0+2]);
        Vec3 v1(vertices[3*vertex1],vertices[3*vertex1+1],vertices[3*vertex1+2]);
        Vec3 v2(vertices[3*vertex2],vertices[3*vertex2+1],vertices[3*vertex2+2]);
        
        obj->vlist_local[vertex0].x = v0.x * scale.x;
        obj->vlist_local[vertex0].y = v0.y * scale.y;
        obj->vlist_local[vertex0].z = v0.z * scale.z;
        
        obj->vlist_local[vertex1].x = v1.x * scale.x;
        obj->vlist_local[vertex1].y = v1.y * scale.y;
        obj->vlist_local[vertex1].z = v1.z * scale.z;
        
        obj->vlist_local[vertex2].x = v2.x * scale.x;
        obj->vlist_local[vertex2].y = v2.y * scale.y;
        obj->vlist_local[vertex2].z = v2.z * scale.z;
        
        if (needNormal) {
            Vec3 v01 = v1 - v0;
            Vec3 v02 = v2 - v0;
            Vec3 vn;
            Vec3::cross(v01,v02,&vn);

            vertexnor_to_hmn(hmn,vertex0,vn);
            vertexnor_to_hmn(hmn,vertex1,vn);
            vertexnor_to_hmn(hmn,vertex2,vn);
        }
        
        obj->faceIndexs[i].state = FACE_STATE_ACTIVITY;
        obj->faceIndexs[i].vlist = obj->vlist_local;
        obj->faceIndexs[i].mati = obj->mati;
        
        // 最新的索引
        obj->faceIndexs[i].vert[0] = vertex0;
        obj->faceIndexs[i].vert[1] = vertex1;
        obj->faceIndexs[i].vert[2] = vertex2;
    }
    
    compute_object_radius(obj);
    
    if (needNormal) {
        for (int i = 0; i < obj->num_vertices; ++i) {
            vector<Vec3>* tmpv = get_vector_from_hmn(hmn,i);
            if (nullptr != tmpv) {
                Vec3 tnv;
                for (int k = 0; k < tmpv->size(); ++k) {
                    tnv += (*tmpv)[k];
                }
                tnv.normalize();
                obj->vlist_local[i].n = tnv;
            }else{
                printf("顶点:%d   无法线.......\n",i);
            }
        }
        
        // 释放内存
        auto it = hmn.begin();
        while (it != hmn.end()) {
            delete it->second;
            ++it;
        }
        hmn.clear();
    }
    
    return (1);
}

