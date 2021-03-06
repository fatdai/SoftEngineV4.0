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


// 渲染文字

int main(int argc, const char * argv[])
{
    if (!initEngine("/Users/user/work/xcode_workspace/SoftEngineV4.0/Resources/")) {
        printf("init error!\n");
        exit(0);
    }
    
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
    Mesh monkey;
    load_Obj_Vertex(&monkey,"monkey.obj",Vec3(80, 80, 80),Vec3(100, 0, 0));
    
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        
        FPS::getInstance()->calculateDt();
        
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit= true;
            }
        }
        
        //逻辑
        {
            angle_y += 0.8;
            angle_x += 0.4;
            if (angle_y > 360.0) {
                angle_y -= 360.0;
            }
            
            if (angle_x > 360.0) {
                angle_x -= 360.0;
            }
            
            renderlist.reset();
            monkey.reset();
        
            monkey.setRotateY(angle_y);
            monkey.setRotateX(angle_x);
            monkey.modelToWorld(camera);
            renderlist.insertMesh(&monkey);
            
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
            
            // 显示Hello world
            Text2D::showText("Hello World",100, 300);
            
            swapBuffer();
        };
        
        delayEngine();
    }
    closeEngine();
    return 0;
}
#endif