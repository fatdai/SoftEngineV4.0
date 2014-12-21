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


#define kNumFace 1
Face testFace[2*kNumFace];

void initPoly(){
    // 随即绘制100个三角形
    for (int i = 0; i < kNumFace; ++i) {
        
        int x0 = 200;
        int y0 = 400;
        
        int x1 = 600;
        int y1 = 400;
        
        int x2 = 600;
        int y2 = 100;
        
        int x3 = 200;
        int y3 = 100;
        
        Face face;
        face.vlist_trans[0].x = x0;
        face.vlist_trans[0].y = y0;
        face.lit_color[0] = Color(255, 0, 0);
        
        face.vlist_trans[1].x = x1;
        face.vlist_trans[1].y = y1;
        face.lit_color[1] = Color(0,255, 0);
        
        face.vlist_trans[2].x = x2;
        face.vlist_trans[2].y = y2;
        face.lit_color[2] = Color(0, 0,255);
        
        testFace[2*i] = face;
        
        /////////////////////////////////////
        Face face1;
        face1.vlist_trans[0].x = x0;
        face1.vlist_trans[0].y = y0;
        face1.lit_color[0] = Color(255, 0, 0);
        
        face1.vlist_trans[1].x = x2;
        face1.vlist_trans[1].y = y2;
        face1.lit_color[1] = Color(0,255, 0);
        
        face1.vlist_trans[2].x = x3;
        face1.vlist_trans[2].y = y3;
        face1.lit_color[2] = Color(0, 0,255);
        testFace[2*i+1] = face1;

    }
}

int main(int argc, const char * argv[])
{
    if (!initEngine("/Users/user/work/xcode_workspace/SoftEngineV4.0/Resources/")) {
        printf("init error!\n");
        exit(0);
    }
    
    initPoly();
    
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
    
    RenderList renderlist;
    Mesh ball;
    SET_BIT(ball.mati.mati_type,Material::GOURAUD);
    load_Obj_Vertex(&ball,"ball.obj",Vec3(100, 100, 100),Vec3(0, 0,-100),true);
    
    
    Color ambient(100,100,100);
    Color diffuse(255,255,255);
    Color specular(255,255,255);
    Vec3  lightDir(0,-1,0);
    
    Point3 lightPosition(0,800,0);
    
    int ambientIndex = LightManager::getInstance()->createAmbientLight(Color(100,100,100));
    
    int dirLightIndex = LightManager::getInstance()->createLight(LIGHT_STATE_ON,LIGHT_ATTR_POINT, ambient, diffuse, specular,&lightPosition,&lightDir,0.5,0.002, 0, 0, 0, 0);
    
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
                    
                }else if (event.key.keysym.sym == SDLK_RIGHT){
                 
                }
//                else if (event.key.keysym.sym == SDLK_UP){
//                    light1->kl -= 0.001;
//                    if (light1->kl < 0) {
//                        light1->kl = 0;
//                    }
//                }else if (event.key.keysym.sym == SDLK_DOWN){
//                    light1->kl += 0.001;
//                }
                else if (event.key.keysym.sym == SDLK_w){
                    wire = !wire;
                }else if (event.key.keysym.sym == SDLK_r){
                    remove_back_face = !remove_back_face;
                }else if (event.key.keysym.sym == SDLK_p){
                    animation = !animation;
                }else if (event.key.keysym.sym == SDLK_a){
                    // 开启/关闭 环境光
                    Light* ambientlight = LightManager::getInstance()->getLight(ambientIndex);
                    if (ambientlight->state == LIGHT_STATE_ON) {
                        ambientlight->state = LIGHT_STATE_OFF;
                    }else{
                        ambientlight->state = LIGHT_STATE_ON;
                    }
                }else if (event.key.keysym.sym == SDLK_d){
                    // 开启/关闭 方向光
                    Light* dirLight = LightManager::getInstance()->getLight(dirLightIndex);
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
                angle_y += 0.4;
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
            ball.setRotateX(angle_x);
            ball.modelToWorld(camera);
            renderlist.insertMesh(&ball);
            
            if (remove_back_face) {
                renderlist.removeBackface(camera);
            }
            renderlist.worldToCamera(camera);
            renderlist.curDown(camera);
            renderlist.light(camera);
            renderlist.perspective(camera);
            renderlist.toscreen(camera);
        };
        
        //渲染
        {
            renderClear();
            
            if (wire) {
                renderlist.drawWire();
            }else{
                renderlist.drawSolid();
            }
            
            FPS::getInstance()->showFPS();
            
//            for (int i = 0; i < kNumFace; ++i) {
//                draw_gouraud_triangle(&testFace[i]);
//            }

            
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