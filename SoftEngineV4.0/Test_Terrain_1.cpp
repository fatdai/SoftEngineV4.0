//
//  DrawTriangle_Wire.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-20.
//  Copyright (c) 2014年 user. All rights reserved.
//

#if 0

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
#include "Terrain_1.h"



int main(int argc, const char * argv[])
{
    if (!initEngine("/Users/user/work/xcode_workspace/SoftEngineV4.0/Resources/")) {
        printf("init error!\n");
        exit(0);
    }
    
    ////////////////////////////////////////////////////////////
    //创建相机
    Vec3 cam_pos(0,0,300);
    Vec3 cam_dir(-10,0,0);
    float near = 1.0;
    float far = 3000.0;
    float fovy = 90.0;
    
//    float angle_y = 0;
//    float angle_x = 0;
    
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
    Mesh terrain;
    createTerrain(&terrain,"height128.raw",32,Point3(1,1,1),Point3(0,-200,-200));
    terrain.setMaterialType(Material::WIRE);
    
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
                    //angle_y += 1.0;
                }else if (event.key.keysym.sym == SDLK_RIGHT){
                    // pointLight->pos.x += 10;
                    //angle_y -= 1.0;
                }
                else if (event.key.keysym.sym == SDLK_UP){
                    pointLight->pos.z -= 10;
                }else if (event.key.keysym.sym == SDLK_DOWN){
                    pointLight->pos.z += 10;
                }
                else if (event.key.keysym.sym == SDLK_a){
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
            camera->update();
            LightManager::getInstance()->transformLight(camera);
            
            renderlist.reset();
            
            
            //////////////////////////////////////////////
            terrain.reset();
            terrain.modelToWorld(camera);
            renderlist.insertMesh(&terrain);
            
            
            //////////////////////////////////////////////
            renderlist.removeBackface(camera);
            renderlist.worldToCamera(camera);
            renderlist.curDown(camera);
        //    renderlist.light(camera);
            renderlist.perspective(camera);
            renderlist.sort(RenderList::SORT_FACE_FARZ);
            renderlist.toscreen(camera);
        };
        
        //渲染
        {
            renderClear();
            
            renderlist.draw();
            
            FPS::getInstance()->showFPS();

            swapBuffer();
        };
        
        delayEngine();
    }
    closeEngine();
    return 0;
}
#endif