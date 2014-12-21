//
//  DrawTriangle_Wire.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#if 1

#include "App.h"
#include "Global.h"
#include "ZCMath.h"
#include "Camera.h"
#include "RenderList.h"
#include "Face.h"
#include "Mesh.h"
#include "FileUtil.h"
#include "Render_1.h"
#include "TextureCache.h"
#include "Vertex.h"
#include "Color.h"
#include "Material.h"
#include "ObjLoader.h"
#include "FPS.h"
#include "Text2D.h"
#include "Light.h"


// 渲染文字
bool wire = false;
bool remove_back_face = true;
bool animation = true;

#define kFaceCount 100
Face mFace[kFaceCount];
void initPoly(){

    for (int i = 0; i < kFaceCount ; ++i) {
        mFace[i].state = FACE_STATE_ACTIVITY | FACE_STATE_NEED_CUTDOWN;
        mFace[i].mati.mati_type = Material::GOURAUD;
        
        mFace[i].vlist_trans[0].x = rand()%600 - 300;
        mFace[i].vlist_trans[0].y = rand()%800 - 400;
        mFace[i].vlist_trans[0].z = rand()%400 - 200;
        
        mFace[i].vlist_trans[1].x = rand()%600 - 300;
        mFace[i].vlist_trans[1].y = rand()%800 - 400;
        mFace[i].vlist_trans[1].z = rand()%400 - 200;
        
        mFace[i].vlist_trans[2].x = rand()%600 - 300;
        mFace[i].vlist_trans[2].y = rand()%800 - 400;
        mFace[i].vlist_trans[2].z = rand()%400 - 200;
        
        mFace[i].lit_color[0] = Color(255, 0, 0);
        mFace[i].lit_color[1] = Color(0, 255, 0);
        mFace[i].lit_color[2] = Color(0, 0, 255);
    }
    
}

int main(int argc, const char * argv[])
{
    if (!initEngine("/Users/user/work/xcode_workspace/SoftEngineV4.0/Resources/")) {
        printf("init error!\n");
        exit(0);
    }
    
    initPoly();
    
    ////////////////////////////////////////////////////////////
    //创建相机
    Vec3 cam_pos(0,0,300);
    Vec3 cam_dir(0,0,0);
    float near = 1.0;
    float far = 3000.0;
    float fovy = 90.0;
    
    float angle_y = 0;
    float angle_x = 0;
    
    // 球面 UVN 相机
    Camera* camera = new Camera(cam_pos,cam_dir,near,far,fovy,kWINDOW_WIDTH,kWINDOW_HEIGHT,CameraMode::CAM_MODEL_UVN_SPHERICAL);
    // end of 创建相机
    ////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////
    // 创建灯光
    Color ambient(100,100,100);
    Color diffuse(255,255,255);
    Vec3  lightDir(0,-1,0);
    Point3 lightPosition(0,0,50);
    
    // 环境光
    Light* ambientLight = LightManager::getInstance()->createAmbientLight(Color(100,100,100));
    
    // 方向光
    Light* dirLight = LightManager::getInstance()->createDirLight(diffuse, lightDir);
    
    // 点光源
    Color pointDiffuse(200,0,0);
    Light* pointLight = LightManager::getInstance()->createPointLight(pointDiffuse,lightPosition);
    
    // end of 创建灯光
    ////////////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////////////
    // 创建物体
    Texture2* texture = TextureCache::getInstance()->addImage("grid.jpg");
    
    Mesh ball;
    load_Obj_Vertex_Tex(&ball,texture, "cube_tex.obj",Vec3(100, 100, 100),Vec3(0, 0,-100));
    ball.setMaterialType(Material::TEXTURE);
    
    // end of 创建物体
    ////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////
    //  开始游戏循环
    RenderList renderlist;
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        
        FPS::getInstance()->calculateDt();
        
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit= true;
            }
            
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_LEFT) {
                    pointLight->pos.x -= 10;
                }else if (event.key.keysym.sym == SDLK_RIGHT){
                    pointLight->pos.x += 10;
                }
                else if (event.key.keysym.sym == SDLK_UP){
                    pointLight->pos.z -= 10;
                }else if (event.key.keysym.sym == SDLK_DOWN){
                    pointLight->pos.z += 10;
                }
                else if (event.key.keysym.sym == SDLK_w){
                    wire = !wire;
                }else if (event.key.keysym.sym == SDLK_r){
                    remove_back_face = !remove_back_face;
                }else if (event.key.keysym.sym == SDLK_p){
                    animation = !animation;
                }else if (event.key.keysym.sym == SDLK_a){
                    // 开启/关闭 环境光
                    if (ambientLight->state == LIGHT_STATE_ON) {
                        ambientLight->state = LIGHT_STATE_OFF;
                    }else{
                        ambientLight->state = LIGHT_STATE_ON;
                    }
                }else if (event.key.keysym.sym == SDLK_d){
                    // 开启/关闭 方向光
                    if (dirLight->state == LIGHT_STATE_ON) {
                        dirLight->state = LIGHT_STATE_OFF;
                    }else{
                        dirLight->state = LIGHT_STATE_ON;
                    }
                }
            }
        }
        
        //逻辑
        {
            if (animation) {
                angle_y += 0.8;
                angle_x += 0.2;
                if (angle_y > 360.0) {
                    angle_y -= 360.0;
                }
                
                if (angle_x > 360.0) {
                    angle_x -= 360.0;
                }
            }
            
            camera->update();
            LightManager::getInstance()->transformLight(camera);
            
            renderlist.reset();
            ball.reset();
            
            ball.setRotateY(angle_y);
            //   ball.setRotateX(angle_x);
            ball.modelToWorld(camera);
            renderlist.insertMesh(&ball);
            
            // 插入多边形
            for (int i = 0; i < kFaceCount; ++i) {
                renderlist.insertFace(&mFace[i]);
            }
            
            
            if (remove_back_face) {
                renderlist.removeBackface(camera);
            }
            renderlist.worldToCamera(camera);
            renderlist.curDown(camera);
            renderlist.light(camera);
            renderlist.perspective(camera);
            renderlist.sort(RenderList::SORT_FACE_FARZ);
            renderlist.toscreen(camera);
        };
        
        //渲染
        {
            renderClear();
            
            renderlist.draw();
            
            FPS::getInstance()->showFPS();
            
            if (wire) {
                Text2D::showText("Render mode:Wire", 0, 90);
            }else{
                Text2D::showText("Render mode:Soild", 0, 90);
            }
            
            if (remove_back_face) {
                Text2D::showText("remove backface:true", 0, 120);
            }else{
                Text2D::showText("remove backface:false", 0, 120);
            }
            
            swapBuffer();
        };
        
        delayEngine();
    }
    closeEngine();
    return 0;
}
#endif