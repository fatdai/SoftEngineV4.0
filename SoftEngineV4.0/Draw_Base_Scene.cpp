//
//  Draw_Soild_Monkey.cpp
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


// 测试下 3d 裁减

void forward(Camera* camera,float speed){
    float rad = MATH_DEG_TO_RAD(camera->dir.y);
    float dx = sinf(rad) * speed;
    float dz = cosf(rad) * speed;
    camera->pos -= Point3(dx,0,dz);
    camera->setPosition(camera->pos);
}

int main(int argc, const char * argv[])
{
    if (!initEngine("/Users/user/work/xcode_workspace/SoftEngineV4.0/Resources/")) {
        printf("init error!\n");
        exit(0);
    }
    
    //创建相机
    Vec3 cam_pos(0,300,300);
    Vec3 cam_dir(-10,0,0);
    float near = 10.0;
    float far = 2000.0;
    float fovy = 90.0;
    
    float angle_y = 0;
    
    // 球面 UVN 相机
    Camera* camera = new Camera(cam_pos,cam_dir,near,far,fovy,kWINDOW_WIDTH,kWINDOW_HEIGHT,CameraMode::CAM_MODEL_UVN_SPHERICAL);
    
    RenderList renderlist;
//    Mesh monkey;
//    load_Obj_Vertex(&monkey,"monkey.obj",Vec3(80, 80, 80),Vec3(200, 0, 0));
//    
//    Mesh ball;
//    load_Obj_Vertex(&ball,"ball.obj",Vec3(100, 100, 100),Vec3(-200, 0, 0));
    
    Mesh ground;
    load_Obj_Vertex(&ground,"ground.obj",Vec3(2000, 2000, 2000),Vec3(0, 0, 0));
    
    Mesh boat;
    load_Obj_Vertex(&boat,"boat.obj",Vec3(100, 100, 100),Vec3(0, 0, 0));
    boat.setColor(Color(255, 0, 0));
    
    Mesh tower;
    load_Obj_Vertex(&tower,"tower.obj",Vec3(80, 80, 80),Vec3(-300, 0, 0));

#define kTowerNumber 1
#define kWorldWidth  200
    Point3 towerPoints[kTowerNumber];
    for (int i = 0; i < kTowerNumber ;++i) {
        towerPoints[i].x = rand() % (2*kWorldWidth) - kWorldWidth;
        towerPoints[i].y = 0;
        towerPoints[i].z = rand() % (2*kWorldWidth) - kWorldWidth;
    }
    
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
                    camera->dir.y += 1;
                    camera->setDir(camera->dir);
                }else if (event.key.keysym.sym == SDLK_RIGHT){
                    camera->dir.y -= 1;
                    camera->setDir(camera->dir);
                }else if (event.key.keysym.sym == SDLK_UP){
                    forward(camera,20);
                }else if (event.key.keysym.sym == SDLK_DOWN){
                    forward(camera,-20);
                }
            }
        }
        
        //逻辑
        {
            angle_y += 1.0;
            if (angle_y > 360.0) {
                angle_y -= 360.0;
            }
            
            camera->update();
            renderlist.reset();
            
            // tower
            for (int i = 0; i < kTowerNumber; ++i) {
                tower.reset();
                tower.world_pos = towerPoints[i];
                tower.setRotateY(angle_y);
                tower.modelToWorld(camera);
                renderlist.insertMesh(&tower);
            }
            
            // ground
            ground.reset();
            ground.modelToWorld(camera);
            renderlist.insertMesh(&ground);
//
//            // boat
//            boat.reset();
//            boat.modelToWorld(camera);
//            renderlist.insertMesh(&boat);
            
            // 进入流水线处理
            renderlist.removeBackface(camera);
            renderlist.worldToCamera(camera);
            renderlist.curDown(camera);
            renderlist.perspective(camera);
            renderlist.toscreen(camera);
        };
        
        //渲染
        {
            renderClear();
            
            renderlist.drawWire();
            
            FPS::getInstance()->showFPS();
            
            Text2D::showText("Press arrow to foward , back , left ,right", 0,60);
            
            swapBuffer();
        };
        
        delayEngine();
    }
    closeEngine();
    return 0;
}
#endif