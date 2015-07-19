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
#include "Render_2.h"
#include "RenderTriangle2D.h"


// 渲染文字
bool wire = false;
bool remove_back_face = true;
bool animation = true;


int main(int argc, const char * argv[])
{
    if (!initEngine("/Users/dai/Graphics/SoftEngineV4.0/Resources/")) {
        printf("init error!\n");
        exit(0);
    }
    
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
    Color pointDiffuse(255,255,0);
    Light* pointLight = LightManager::getInstance()->createPointLight(pointDiffuse,lightPosition);
    
    // end of 创建灯光
    ////////////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////////////
    // 创建物体
    Texture2* texture = TextureCache::getInstance()->addImage("plane.jpg");
    
    Mesh ball;
    ball.setMaterialType(Material::TEXTURE);
    load_Obj_Vertex_Tex(&ball,texture, "cube_tex.obj",Vec3(100, 100, 100),Vec3(0, 0,-100));
  //  load_Obj_Vertex(&ball, "monkey.obj",Vec3(150, 150, 150),Vec3(0, 0,-100));
    
    
 //   Texture2* planeTex = TextureCache::getInstance()->addImage("plane.jpg");
    Mesh plane;
    plane.setMaterialType(Material::TEXTURE);
    load_Obj_Vertex_Tex(&plane,texture,"my_plane.obj",Vec3(100, 100, 100), Vec3(0, 0,0));
    
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
                   // pointLight->pos.x -= 10;
                    angle_y += 1.0;
                }else if (event.key.keysym.sym == SDLK_RIGHT){
                   // pointLight->pos.x += 10;
                    angle_y -= 1.0;
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
          //  ball.world_pos += Point3(0,200,0);
         //   ball.setRotateX(angle_x);
            ball.modelToWorld(camera);
            renderlist.insertMesh(&ball);
            
            
            //////////////////////////////////////////////
            plane.reset();
           // plane.setRotateX(45);
            plane.setRotateX(angle_y);
            plane.modelToWorld(camera);
           // renderlist.insertMesh(&plane);
            
            
            //////////////////////////////////////////////
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
            
            renderlist.draw(wire);
            
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
            

            
            //////////////////////////////////////////////
            // v0-v5
            Vec2 v0[3] = {
                {5.1,84.12},
                {214.24,84.12},
                {143.44,233.6}
            };
            Vec2 v1[3]={
                {323.12,23.524},
                {282.32,224.132},
                {423.12,224.132},
            };
            Vec2 v2[3]={
                {23.12,431.12},
                {296.31,363.12},
                {333.123,313.45},
            };
            Vec2 v3[3]={
                {403.32,313.12},
                {598.31,342.11},
                {732.123,432.11},
            };
            Vec2 v4[3]={
                {98.11,460.11},
                {132.12,593.44},
                {459.11,481.2},
            };
            Vec2 v5[3]={
                {532.11,511.11},
                {513.18,582.36},
                {711.11,463.12},
            };
            
            //v6  v7
            Vec2 v6[3]={
                {510.12,23.14},
                {510.96,123.147},
                {580.96,123.947}
            };
            
            Vec2 v7[3]={
                {610.12,23.14},
                {610.96,125.747},
                {680.96,23.143}
            };
            
            // v8 v9
            Vec2 v8[3]={
                {580.96,223.14},
                {510.96,323.147},
                {580.96,323.947}
            };
            
            Vec2 v9[3]={
                {610.12,223.14},
                {680.96,325.747},
                {680.96,223.143}
            };
            
//            RenderGouraudTriangle2D(v0);
//            RenderGouraudTriangle2D(v1);
//            RenderGouraudTriangle2D(v2);
//            RenderGouraudTriangle2D(v3);
//            RenderGouraudTriangle2D(v4);
//            RenderGouraudTriangle2D(v5);
//            DrawTriangle(v6);
//            DrawTriangle(v7);
//            DrawTriangle(v8);
//            DrawTriangle(v9);
            
            swapBuffer();
        };
        
        delayEngine();
    }
    closeEngine();
    return 0;
}
#endif