//
//  Render_2.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-23.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "Render_2.h"
#include <assert.h>


enum class TRIANGLE_TYPE{
    
    // 平顶
    FLAT_TOP_TRIANGLE = 1,
    
    // 平底
    FLAT_BOTTOM_TRIANGLE,
    
    // 左边长
    LEFT_LONG_TRIANGLE,
    
    // 右边长
    RIGHT_LONG_TRIANGLE,
    
    
};


int ClipLine(int &x1,int &y1,int &x2, int &y2){
    // this function clips the sent line using the globally defined clipping
    // region
    
    // internal clipping codes
#define CLIP_CODE_C  0x0000
#define CLIP_CODE_N  0x0008
#define CLIP_CODE_S  0x0004
#define CLIP_CODE_E  0x0002
#define CLIP_CODE_W  0x0001
    
#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009
#define CLIP_CODE_SW 0x0005
    
    int xc1=x1,
    yc1=y1,
	xc2=x2,
	yc2=y2;
    
    int p1_code=0,
    p2_code=0;
    
    // determine codes for p1 and p2
    if (y1 < kMIN_CLIP_Y)
        p1_code|=CLIP_CODE_N;
    else
        if (y1 > kMAX_CLIP_Y)
            p1_code|=CLIP_CODE_S;
    
    if (x1 < kMIN_CLIP_X)
        p1_code|=CLIP_CODE_W;
    else
        if (x1 > kMAX_CLIP_X)
            p1_code|=CLIP_CODE_E;
    
    if (y2 < kMIN_CLIP_Y)
        p2_code|=CLIP_CODE_N;
    else
        if (y2 > kMAX_CLIP_Y)
            p2_code|=CLIP_CODE_S;
    
    if (x2 < kMIN_CLIP_X)
        p2_code|=CLIP_CODE_W;
    else
        if (x2 > kMAX_CLIP_X)
            p2_code|=CLIP_CODE_E;
    
    // try and trivially reject
    if ((p1_code & p2_code))
        return(0);
    
    // test for totally visible, if so leave points untouched
    if (p1_code==0 && p2_code==0)
        return(1);
    
    // determine end clip point for p1
    switch(p1_code)
    {
        case CLIP_CODE_C: break;
            
        case CLIP_CODE_N:
        {
            yc1 = kMIN_CLIP_Y;
            xc1 = x1 + 0.5+(kMIN_CLIP_Y-y1)*(x2-x1)/(y2-y1);
        } break;
        case CLIP_CODE_S:
        {
            yc1 = kMAX_CLIP_Y;
            xc1 = x1 + 0.5+(kMAX_CLIP_Y-y1)*(x2-x1)/(y2-y1);
        } break;
            
        case CLIP_CODE_W:
        {
            xc1 = kMIN_CLIP_X;
            yc1 = y1 + 0.5+(kMIN_CLIP_X-x1)*(y2-y1)/(x2-x1);
        } break;
            
        case CLIP_CODE_E:
        {
            xc1 = kMAX_CLIP_X;
            yc1 = y1 + 0.5+(kMAX_CLIP_X-x1)*(y2-y1)/(x2-x1);
        } break;
            
            // these cases are more complex, must compute 2 intersections
        case CLIP_CODE_NE:
        {
            // north hline intersection
            yc1 = kMIN_CLIP_Y;
            xc1 = x1 + 0.5+(kMIN_CLIP_Y-y1)*(x2-x1)/(y2-y1);
            
            // test if intersection is valid, of so then done, else compute next
			if (xc1 < kMIN_CLIP_X || xc1 > kMAX_CLIP_X)
            {
				// east vline intersection
				xc1 = kMAX_CLIP_X;
				yc1 = y1 + 0.5+(kMAX_CLIP_X-x1)*(y2-y1)/(x2-x1);
            } // end if
            
        } break;
            
        case CLIP_CODE_SE:
        {
            // south hline intersection
            yc1 = kMAX_CLIP_Y;
            xc1 = x1 + 0.5+(kMAX_CLIP_Y-y1)*(x2-x1)/(y2-y1);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc1 < kMIN_CLIP_X || xc1 > kMAX_CLIP_X)
            {
                // east vline intersection
                xc1 = kMAX_CLIP_X;
                yc1 = y1 + 0.5+(kMAX_CLIP_X-x1)*(y2-y1)/(x2-x1);
            } // end if
            
        } break;
            
        case CLIP_CODE_NW:
        {
            // north hline intersection
            yc1 = kMIN_CLIP_Y;
            xc1 = x1 + 0.5+(kMIN_CLIP_Y-y1)*(x2-x1)/(y2-y1);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc1 < kMIN_CLIP_X || xc1 > kMAX_CLIP_X)
            {
                xc1 = kMIN_CLIP_X;
                yc1 = y1 + 0.5+(kMIN_CLIP_X-x1)*(y2-y1)/(x2-x1);
            } // end if
            
        } break;
            
        case CLIP_CODE_SW:
        {
            // south hline intersection
            yc1 = kMAX_CLIP_Y;
            xc1 = x1 + 0.5+(kMAX_CLIP_Y-y1)*(x2-x1)/(y2-y1);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc1 < kMIN_CLIP_X || xc1 > kMAX_CLIP_X)
            {
                xc1 = kMIN_CLIP_X;
                yc1 = y1 + 0.5+(kMIN_CLIP_X-x1)*(y2-y1)/(x2-x1);
            } // end if
            
        } break;
            
        default:break;
            
    } // end switch
    
    // determine clip point for p2
    switch(p2_code)
    {
        case CLIP_CODE_C: break;
            
        case CLIP_CODE_N:
        {
            yc2 = kMIN_CLIP_Y;
            xc2 = x2 + (kMIN_CLIP_Y-y2)*(x1-x2)/(y1-y2);
        } break;
            
        case CLIP_CODE_S:
        {
            yc2 = kMAX_CLIP_Y;
            xc2 = x2 + (kMAX_CLIP_Y-y2)*(x1-x2)/(y1-y2);
        } break;
            
        case CLIP_CODE_W:
        {
            xc2 = kMIN_CLIP_X;
            yc2 = y2 + (kMIN_CLIP_X-x2)*(y1-y2)/(x1-x2);
        } break;
            
        case CLIP_CODE_E:
        {
            xc2 = kMAX_CLIP_X;
            yc2 = y2 + (kMAX_CLIP_X-x2)*(y1-y2)/(x1-x2);
        } break;
            
            // these cases are more complex, must compute 2 intersections
        case CLIP_CODE_NE:
        {
            // north hline intersection
            yc2 = kMIN_CLIP_Y;
            xc2 = x2 + 0.5+(kMIN_CLIP_Y-y2)*(x1-x2)/(y1-y2);
            
            // test if intersection is valid, of so then done, else compute next
			if (xc2 < kMIN_CLIP_X || xc2 > kMAX_CLIP_X)
            {
				// east vline intersection
				xc2 = kMAX_CLIP_X;
				yc2 = y2 + 0.5+(kMAX_CLIP_X-x2)*(y1-y2)/(x1-x2);
            } // end if
            
        } break;
            
        case CLIP_CODE_SE:
        {
            // south hline intersection
            yc2 = kMAX_CLIP_Y;
            xc2 = x2 + 0.5+(kMAX_CLIP_Y-y2)*(x1-x2)/(y1-y2);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc2 < kMIN_CLIP_X || xc2 > kMAX_CLIP_X)
            {
                // east vline intersection
                xc2 = kMAX_CLIP_X;
                yc2 = y2 + 0.5+(kMAX_CLIP_X-x2)*(y1-y2)/(x1-x2);
            } // end if
            
        } break;
            
        case CLIP_CODE_NW:
        {
            // north hline intersection
            yc2 = kMIN_CLIP_Y;
            xc2 = x2 + 0.5+(kMIN_CLIP_Y-y2)*(x1-x2)/(y1-y2);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc2 < kMIN_CLIP_X || xc2 > kMAX_CLIP_X)
            {
                xc2 = kMIN_CLIP_X;
                yc2 = y2 + 0.5+(kMIN_CLIP_X-x2)*(y1-y2)/(x1-x2);
            } // end if
            
        } break;
            
        case CLIP_CODE_SW:
        {
            // south hline intersection
            yc2 = kMAX_CLIP_Y;
            xc2 = x2 + 0.5+(kMAX_CLIP_Y-y2)*(x1-x2)/(y1-y2);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc2 < kMIN_CLIP_X || xc2 > kMAX_CLIP_X)
            {
                xc2 = kMIN_CLIP_X;
                yc2 = y2 + 0.5+(kMIN_CLIP_X-x2)*(y1-y2)/(x1-x2);
            } // end if
            
        } break;
            
        default:break;
            
    } // end switch
    
    // do bounds check
    if ((xc1 < kMIN_CLIP_X) || (xc1 > kMAX_CLIP_X) ||
        (yc1 < kMIN_CLIP_Y) || (yc1 > kMAX_CLIP_Y) ||
        (xc2 < kMIN_CLIP_X) || (xc2 > kMAX_CLIP_X) ||
        (yc2 < kMIN_CLIP_Y) || (yc2 > kMAX_CLIP_Y) )
	{
        return(0);
	} // end if
    
    // store vars back
    x1 = xc1;
    y1 = yc1;
    x2 = xc2;
    y2 = yc2;
    
    return(1);
}

void DrawLine(int x0,int y0,int x1,int y1,Uint32 color){
    
    if (ClipLine(x0, y0, x1, y1) == 0) {
        return;
    }
    
    int pitchWidth = gSurface->pitch >> 2;
    
    int dx = x1 - x0;
    int dy = y1 - y0;
    
    // test x
    int x_inc;
    if (dx >= 0) {
        x_inc = 1;
    }else{
        x_inc = -1;
        dx = -dx;
    }
    
    // test y
    int y_inc;
    if (dy >= 0) {
        y_inc = pitchWidth;
    }else{
        y_inc = -pitchWidth;
        dy = -dy;
    }
    
    // compute (dx,dy) * 2
    int dx2 = dx << 1;
    int dy2 = dy << 1;
    
    // pre-compute first pixel address in video buffer based on 32bit data
    Uint32* vb_start = (Uint32 *)gSurface->pixels + x0 + y0 * pitchWidth;
    
    int error;
    
    // now based on which delta is greater we can draw the line
    if (dx > dy) {
        
        // initialize error term
        error = dy2 - dx;
        
        for (int index = 0; index <= dx; ++index) {
            // set pixel
            *vb_start = color;
            
            //test if error has overflowed
            if (error >= 0) {
                error-=dx2;
                
                //move to next line
                vb_start += y_inc;
            }
            
            // adjust the error term
            error += dy2;
            
            vb_start += x_inc;
        } // end for
    }else{
        // initialize error term
        error = dx2 - dy;
        
        // draw the line
        for (int index=0; index <= dy; index++)
        {
            // set the pixel
            *vb_start = color;
            
            // test if error overflowed
            if (error >= 0)
            {
                error-=dy2;
                
                // move to next line
                vb_start+=x_inc;
                
            } // end if error overflowed
            
            // adjust the error term
            error+=dx2;
            
            // move to the next pixel
            vb_start+=y_inc;
            
        } // end for
    }
}


void DrawTriangle(Face* face){
    
    Uint32 color = face->lit_color[0].toInt_RGB();
    
    // first trivial clipping rejection tests
    if (((face->vlist_trans[0].y < kMIN_CLIP_Y)  &&
         (face->vlist_trans[1].y < kMIN_CLIP_Y)  &&
         (face->vlist_trans[2].y < kMIN_CLIP_Y)) ||
        
        ((face->vlist_trans[0].y > kMAX_CLIP_Y)  &&
         (face->vlist_trans[1].y > kMAX_CLIP_Y)  &&
         (face->vlist_trans[2].y > kMAX_CLIP_Y)) ||
        
        ((face->vlist_trans[0].x < kMIN_CLIP_X)  &&
         (face->vlist_trans[1].x < kMIN_CLIP_X)  &&
         (face->vlist_trans[2].x < kMIN_CLIP_X)) ||
        
        ((face->vlist_trans[0].x > kMAX_CLIP_X)  &&
         (face->vlist_trans[1].x > kMAX_CLIP_X)  &&
         (face->vlist_trans[2].x > kMAX_CLIP_X)))
    {
        return;
    }
    
    
    // degenerate triangle
    if ( ((face->vlist_trans[0].x==face->vlist_trans[1].x) &&
          (face->vlist_trans[1].x==face->vlist_trans[2].x)) ||
        ((face->vlist_trans[0].y==face->vlist_trans[1].y) &&
         (face->vlist_trans[1].y==face->vlist_trans[2].y)))
    {
        return;
    }
    
    // 排列顶点
    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp;
    
    // 注意,这里不能换成 int 类型去比较
    if (face->vlist_trans[v1].y < face->vlist_trans[v0].y) {
        temp = v0;
        v0 = v1;
        v1 = temp;
    }
    
    if (face->vlist_trans[v2].y < face->vlist_trans[v0].y){
        temp = v0;
        v0 = v2;
        v2 = temp;
    }
    
    if (face->vlist_trans[v2].y < face->vlist_trans[v1].y) {
        temp = v1;
        v1 = v2;
        v2 = temp;
    }
    
    TRIANGLE_TYPE tri_type;
    
    // 检测是哪种三角形
    if (face->vlist_trans[v0].y == face->vlist_trans[v1].y) {
        // 平顶三角形
        tri_type = TRIANGLE_TYPE::FLAT_TOP_TRIANGLE;
        
        // 从左到右排列顶点
        if (face->vlist_trans[v1].x < face->vlist_trans[v0].x) {
            temp = v0;
            v0 = v1;
            v1 = temp;
        }
    }//end if
    else{
        if (face->vlist_trans[v1].y == face->vlist_trans[v2].y) {
            // 平底三角形
            tri_type = TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE;
            
            //从左到右
            if (face->vlist_trans[v2].x < face->vlist_trans[v1].x) {
                temp = v1;
                v1 = v2;
                v2 = temp;
            }
        }// end if
        else{
            // 判断三角形是左边长还是右边长
            // 判断三角形是左边长还是右边长
            // 插值计算
            float interx = face->vlist_trans[v0].x + (face->vlist_trans[v1].y - face->vlist_trans[v0].y)*(face->vlist_trans[v2].x - face->vlist_trans[v0].x)/(face->vlist_trans[v2].y - face->vlist_trans[v0].y);
            if (interx > face->vlist_trans[v1].x) {
                tri_type = TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE;
            }else{
                tri_type = TRIANGLE_TYPE::LEFT_LONG_TRIANGLE;
            }
        }// end else
    }// end else
    float x0 = face->vlist_trans[v0].x;
    float y0 = face->vlist_trans[v0].y;
    
    float x1 = face->vlist_trans[v1].x;
    float y1 = face->vlist_trans[v1].y;
    
    float x2 = face->vlist_trans[v2].x;
    float y2 = face->vlist_trans[v2].y;
    

    // 提取顶点,准备进行光栅化
    if (tri_type == TRIANGLE_TYPE::FLAT_TOP_TRIANGLE) {
        // 直接绘制
        float xs = x0,xe = x1;
        float dy = y2 - y0;
        float dxdyl = (x2 - x0)/dy;
        float dxdyr = (x2 - x1)/dy;
        
        int endY = y2;
        for (int i = y0; i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdyl;
            xe += dxdyr;
        }
    }else if(tri_type == TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE){
        float xs = x0,xe = x0;
        float dy = y1 - y0;
        float dxdyl = (x1-x0)/dy;
        float dxdyr = (x2-x0)/dy;
        
        int endY = y1;
        for (int i = y0; i < endY;++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdyl;
            xe += dxdyr;
        }
    }
    else if (tri_type == TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        float xs = x0,xe = x0;
        
        //分两段绘制
        int endY = y1;
        for (int i = y0; i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdy10;
            xe += dxdy20;
        }
        
        endY = y2;
        xs = x1;
        for (int i = y1; i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdy21;
            xe += dxdy20;
        }
    }else if (tri_type == TRIANGLE_TYPE::LEFT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        float xs = x0,xe = x0;
        int endY = y1;
        for (int i = y0; i < endY; ++i) {
            DrawLine(xs, i, xe, i, color);
            xs += dxdy20;
            xe += dxdy10;
        }
        
        endY = y2;
        xe = x1;
        for (int i = y1;i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdy20;
            xe += dxdy21;
        }
    }
    
    else{
        printf("DrawTriangle maybe some error......\n");
    }
}

void DrawGouraudTriangle(Face* face){
    
    Uint32 color = face->lit_color[0].toInt_RGB();
    
    // first trivial clipping rejection tests
    if (((face->vlist_trans[0].y < kMIN_CLIP_Y)  &&
         (face->vlist_trans[1].y < kMIN_CLIP_Y)  &&
         (face->vlist_trans[2].y < kMIN_CLIP_Y)) ||
        
        ((face->vlist_trans[0].y > kMAX_CLIP_Y)  &&
         (face->vlist_trans[1].y > kMAX_CLIP_Y)  &&
         (face->vlist_trans[2].y > kMAX_CLIP_Y)) ||
        
        ((face->vlist_trans[0].x < kMIN_CLIP_X)  &&
         (face->vlist_trans[1].x < kMIN_CLIP_X)  &&
         (face->vlist_trans[2].x < kMIN_CLIP_X)) ||
        
        ((face->vlist_trans[0].x > kMAX_CLIP_X)  &&
         (face->vlist_trans[1].x > kMAX_CLIP_X)  &&
         (face->vlist_trans[2].x > kMAX_CLIP_X)))
    {
        return;
    }
    
    
    // degenerate triangle
    if ( ((face->vlist_trans[0].x==face->vlist_trans[1].x) &&
          (face->vlist_trans[1].x==face->vlist_trans[2].x)) ||
        ((face->vlist_trans[0].y==face->vlist_trans[1].y) &&
         (face->vlist_trans[1].y==face->vlist_trans[2].y)))
    {
        return;
    }
    
    // 排列顶点
    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp;
    
    // 注意,这里不能换成 int 类型去比较
    if (face->vlist_trans[v1].y < face->vlist_trans[v0].y) {
        temp = v0;
        v0 = v1;
        v1 = temp;
    }
    
    if (face->vlist_trans[v2].y < face->vlist_trans[v0].y){
        temp = v0;
        v0 = v2;
        v2 = temp;
    }
    
    if (face->vlist_trans[v2].y < face->vlist_trans[v1].y) {
        temp = v1;
        v1 = v2;
        v2 = temp;
    }
    
    TRIANGLE_TYPE tri_type;
    
    // 检测是哪种三角形
    if (face->vlist_trans[v0].y == face->vlist_trans[v1].y) {
        // 平顶三角形
        tri_type = TRIANGLE_TYPE::FLAT_TOP_TRIANGLE;
        
        // 从左到右排列顶点
        if (face->vlist_trans[v1].x < face->vlist_trans[v0].x) {
            temp = v0;
            v0 = v1;
            v1 = temp;
        }
    }//end if
    else{
        if (face->vlist_trans[v1].y == face->vlist_trans[v2].y) {
            // 平底三角形
            tri_type = TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE;
            
            //从左到右
            if (face->vlist_trans[v2].x < face->vlist_trans[v1].x) {
                temp = v1;
                v1 = v2;
                v2 = temp;
            }
        }// end if
        else{
            // 判断三角形是左边长还是右边长
            // 判断三角形是左边长还是右边长
            // 插值计算
            float interx = (face->vlist_trans[v1].y - face->vlist_trans[v0].y)*(face->vlist_trans[v2].x - face->vlist_trans[v0].x)/(face->vlist_trans[v2].y - face->vlist_trans[v0].y);
            if (interx > face->vlist_trans[v1].x) {
                tri_type = TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE;
            }else{
                tri_type = TRIANGLE_TYPE::LEFT_LONG_TRIANGLE;
            }
        }// end else
    }// end else
    
    
    float x0 = face->vlist_trans[v0].x;
    float y0 = face->vlist_trans[v0].y;
    unsigned char x0_r = face->lit_color[0].r;
    unsigned char x0_g = face->lit_color[0].g;
    unsigned char x0_b = face->lit_color[0].b;
    
    float x1 = face->vlist_trans[v1].x;
    float y1 = face->vlist_trans[v1].y;
    unsigned char x1_r = face->lit_color[1].r;
    unsigned char x1_g = face->lit_color[1].g;
    unsigned char x1_b = face->lit_color[1].b;
    
    float x2 = face->vlist_trans[v2].x;
    float y2 = face->vlist_trans[v2].y;
    unsigned char x2_r = face->lit_color[1].r;
    unsigned char x2_g = face->lit_color[1].g;
    unsigned char x2_b = face->lit_color[1].b;
    
    // 提取顶点,准备进行光栅化
    if (tri_type == TRIANGLE_TYPE::FLAT_TOP_TRIANGLE) {
        
        // 直接绘制
        float dy = y2 - y0;
        
        float dxdyl = (x2 - x0)/dy;
        float dxrdyl = (x2_r - x0_r)/dy;
        float dxgdyl = (x2_g - x0_g)/dy;
        float dxbdyl = (x2_b - x0_b)/dy;
        
        float dxdyr = (x2 - x1)/dy;
        float dxrdyr = (x2_r - x1_r)/dy;
        float dxgdyr = (x2_g - x1_g)/dy;
        float dxbdyr = (x2_b - x1_b)/dy;
        
        float xl = x0,yl = y0;
        float r_l = x0_r,g_l = x0_g,b_l = x0_b;
        float r_r = x1_r,g_r = x1_g,b_r = x1_b;
        
        // test rejection
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
           
            y0 = kMIN_CLIP_Y;
            
//            dy = kMIN_CLIP_Y - y0;
//            
//            // 计算新的  xl yl
//            xl = dxdyl * dy  + x0;
//            r_l = dxrdyl * dy + x0_r;
//            g_l = dxgdyl * dy + x0_g;
//            b_l = dxbdyl * dy + x0_b;
        }
        
        // test y2
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }
        
        // 开始循环
        int endY = y2;
        Uint32* dst_buffer = (Uint32 *)gSurface->pixels;
        int mem_pitch = gSurface->pitch >> 2;
        
        int xs = x0,xe = x1;
        int start_buf = 0;
        for (int i = y0; i < endY; ++i) {
            
            assert(xs <= xe);
            
            // test xs
            if (xs > kMAX_CLIP_X) {
                xs += dxdyl;
                xe += dxdyr;
                continue;
            }
            
            if (xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            // test xe
            if (xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            start_buf = i * mem_pitch + xs;
            int column = xe - xs;
            for (int j = 0; j <= column; ++j) {
                dst_buffer[start_buf + j] = color;
            }

            xs += dxdyl;
            xe += dxdyr;
        }
    }else if(tri_type == TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE){
        float xs = x0,xe = x0;
        float dy = y1 - y0;
        float dxdyl = (x1-x0)/dy;
        float dxdyr = (x2-x0)/dy;
        
        int endY = y1;
        for (int i = y0; i < endY;++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdyl;
            xe += dxdyr;
        }
    }
    else if (tri_type == TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        float xs = x0,xe = x0;
        
        //分两段绘制
        int endY = y1;
        for (int i = y0; i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdy10;
            xe += dxdy20;
        }
        
        endY = y2;
        xe = y1;
        for (int i = y1; i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdy21;
            xe += dxdy20;
        }
    }else if (tri_type == TRIANGLE_TYPE::LEFT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        float xs = x0,xe = x0;
        int endY = y1;
        for (int i = y0; i < endY; ++i) {
            DrawLine(xs, i, xe, i, color);
            xs += dxdy20;
            xe += dxdy10;
        }
        
        endY = y2;
        xe = x1;
        for (int i = y1;i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdy20;
            xe += dxdy21;
        }
    }
    
    else{
        printf("DrawTriangle maybe some error......\n");
    }
}


#ifdef _DEBUG_RASTER_


#pragma mark 测试绘制 gouraud 三角形

void DrawGouraudTriangle(Vec2* pv){
    
    Uint32 color = gColorRed;
    
    int ix0 = (int)pv[0].x;
    int iy0 = (int)pv[0].y;
    int ix1 = (int)pv[1].x;
    int iy1 = (int)pv[1].y;
    int ix2 = (int)pv[2].x;
    int iy2 = (int)pv[2].y;
    
    // first trivial clipping rejection tests
    if (((pv[0].y < kMIN_CLIP_Y)  &&
         (pv[1].y < kMIN_CLIP_Y)  &&
         (pv[2].y < kMIN_CLIP_Y)) ||
        
        ((pv[0].y > kMAX_CLIP_Y)  &&
         (pv[1].y > kMAX_CLIP_Y)  &&
         (pv[2].y > kMAX_CLIP_Y)) ||
        
        ((pv[0].x < kMIN_CLIP_X)  &&
         (pv[1].x < kMIN_CLIP_X)  &&
         (pv[2].x < kMIN_CLIP_X)) ||
        
        ((pv[0].x > kMAX_CLIP_X)  &&
         (pv[1].x > kMAX_CLIP_X)  &&
         (pv[2].x > kMAX_CLIP_X)))
    {
        return;
    }
    
    
    // degenerate triangle
    if ( ((ix0==ix1) &&
          (ix1==ix2)) ||
        ((iy0==iy1) &&
         (iy1==iy2)))
    {
        return;
    }
    
    // 排列顶点
    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp;
    if (iy1 < iy0) {
        temp = v0;
        v0 = v1;
        v1 = temp;
    }
    
    if (iy2 < iy0){
        temp = v0;
        v0 = v2;
        v2 = temp;
    }
    
    if (iy2 < iy1) {
        temp = v1;
        v1 = v2;
        v2 = temp;
    }
    
    TRIANGLE_TYPE tri_type;
    
    // 检测是哪种三角形
    if ((int)pv[v0].y == (int)pv[v1].y) {
        // 平顶三角形
        tri_type = TRIANGLE_TYPE::FLAT_TOP_TRIANGLE;
        
        // 从左到右排列顶点
        if (ix1 < ix0) {
            temp = v0;
            v0 = v1;
            v1 = temp;
        }
    }//end if
    else{
        if ((int)pv[v1].y == (int)pv[v2].y) {
            // 平底三角形
            tri_type = TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE;
            
            //从左到右
            if (ix1 < ix2) {
                temp = v1;
                v1 = v2;
                v2 = temp;
            }
        }// end if
        else{
            // 判断三角形是左边长还是右边长
            // 插值计算
            float interx = pv[v0].x + (pv[v1].y - pv[v0].y)*(pv[v2].x - pv[v0].x)/(pv[v2].y - pv[v0].y);
            if (interx > pv[v1].x) {
                tri_type = TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE;
            }else{
                tri_type = TRIANGLE_TYPE::LEFT_LONG_TRIANGLE;
            }
        }// end else
    }// end else
    
    float x0 = pv[v0].x;
    float y0 = pv[v0].y;
    
    float x1 = pv[v1].x;
    float y1 = pv[v1].y;
    
    float x2 = pv[v2].x;
    float y2 = pv[v2].y;
    
    
    Uint32* dst_buffer = (Uint32*)gSurface->pixels;
    int mem_pitch = gSurface->pitch >> 2;
    
    // 提取顶点,准备进行光栅化
    if (tri_type == TRIANGLE_TYPE::FLAT_TOP_TRIANGLE) {

        float dy = y2 - y0;
        float dxdyl = (x2 - x0)/dy;
        float dxdyr = (x2 - x1)/dy;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
        }
        
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }
        
        int endY = y2;
        float xs = x0,xe = x1;
        float old_xs = x0,old_xe = x1;
        int start = (int)y0 * mem_pitch;
        for (int i = y0; i < endY; ++i) {
            
            assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
        
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
            
        }
    }else if(tri_type == TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE){
        
        
        float dy = y1 - y0;
        float dxdyl = (x1-x0)/dy;
        float dxdyr = (x2-x0)/dy;
        
        // 添加裁减代码
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
        }
        
        if (y1 > kMAX_CLIP_Y) {
            y1 = kMAX_CLIP_Y;
        }
        
        float xs = x0,xe = x0;
        float old_xs = x0,old_xe = x0;
        int endY = y1;
        int start = (int)y0 * mem_pitch;
        for (int i = y0; i < endY;++i) {
            
            assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdyl;
                old_xe += dxdyr;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdyl;
            old_xe += dxdyr;
            xs = old_xs;
            xe = old_xe;
        }
    }
    
    else if (tri_type == TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
        }
        
        // test y1
        if (y1 < kMIN_CLIP_Y) {
            y1 = kMIN_CLIP_Y;
        }
        
        if (y1 > kMAX_CLIP_Y) {
            y1 = kMAX_CLIP_Y;
        }
        
        // test y2
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }
        

        float xs = x0,xe = x0;
        float old_xs = x0,old_xe = x0;
        int start = (int)y0 * mem_pitch;
        
        //分两段绘制
        int endY = y1;
        for (int i = y0; i < endY; ++i) {
            
         //   assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy10;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy10;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
        }
        
        endY = y2;
        xs = x1;
        old_xs = x1;
        for (int i = y1; i < endY; ++i) {
            
          //  assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy21;
                old_xe += dxdy20;
                xs = old_xs;
                xe = old_xe;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy21;
            old_xe += dxdy20;
            xs = old_xs;
            xe = old_xe;
        }
    }else if (tri_type == TRIANGLE_TYPE::LEFT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        // 裁减
        if (y0 > kMAX_CLIP_Y || y2 < kMIN_CLIP_Y) {
            return;
        }
        
        if (y0 < kMIN_CLIP_Y) {
            y0 = kMIN_CLIP_Y;
        }
        
        // test y1
        if (y1 < kMIN_CLIP_Y) {
            y1 = kMIN_CLIP_Y;
        }
        
        if (y1 > kMAX_CLIP_Y) {
            y1 = kMAX_CLIP_Y;
        }
        
        // test y2
        if (y2 > kMAX_CLIP_Y) {
            y2 = kMAX_CLIP_Y;
        }

        float xs = x0,xe = x0;
        float old_xs = x0,old_xe = x0;
        int endY = y1;
        int start = (int)y0 * mem_pitch;
        for (int i = y0; i < endY; ++i) {
            
          //  assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy10;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy10;
            xs = old_xs;
            xe = old_xe;
        }
        
        endY = y2;
        xe = x1;
        old_xe = x1;
        for (int i = y1;i < endY; ++i) {
            
         //   assert(old_xs <= old_xe);
            
            if (old_xs > kMAX_CLIP_X || old_xe < kMIN_CLIP_X) {
                old_xs += dxdy20;
                old_xe += dxdy21;
                continue;
            }
            
            if (old_xs < kMIN_CLIP_X) {
                xs = kMIN_CLIP_X;
            }
            
            if (old_xe > kMAX_CLIP_X) {
                xe = kMAX_CLIP_X;
            }
            
            for (int k = xs; k <= xe; ++k) {
                dst_buffer[start + k] = color;
            }
            start += mem_pitch;
            
            old_xs += dxdy20;
            old_xe += dxdy21;
            xs = old_xs;
            xe = old_xe;
        }
    }
    else{
        printf("DrawTriangle maybe some error......\n");
    }
}

void DrawTriangle(Vec2* pv){
    
    Uint32 color = gColorGreen;
    
    int ix0 = (int)pv[0].x;
    int iy0 = (int)pv[0].y;
    int ix1 = (int)pv[1].x;
    int iy1 = (int)pv[1].y;
    int ix2 = (int)pv[2].x;
    int iy2 = (int)pv[2].y;
    
    // first trivial clipping rejection tests
    if (((pv[0].y < kMIN_CLIP_Y)  &&
         (pv[1].y < kMIN_CLIP_Y)  &&
         (pv[2].y < kMIN_CLIP_Y)) ||
        
        ((pv[0].y > kMAX_CLIP_Y)  &&
         (pv[1].y > kMAX_CLIP_Y)  &&
         (pv[2].y > kMAX_CLIP_Y)) ||
        
        ((pv[0].x < kMIN_CLIP_X)  &&
         (pv[1].x < kMIN_CLIP_X)  &&
         (pv[2].x < kMIN_CLIP_X)) ||
        
        ((pv[0].x > kMAX_CLIP_X)  &&
         (pv[1].x > kMAX_CLIP_X)  &&
         (pv[2].x > kMAX_CLIP_X)))
    {
        return;
    }
    

    // degenerate triangle
    if ( ((ix0==ix1) &&
          (ix1==ix2)) ||
        ((iy0==iy1) &&
         (iy1==iy2)))
    {
        return;
    }
    
    // 排列顶点
    int v0 = 0;
    int v1 = 1;
    int v2 = 2;
    int temp;
    if (iy1 < iy0) {
        temp = v0;
        v0 = v1;
        v1 = temp;
    }
    
    if (iy2 < iy0){
        temp = v0;
        v0 = v2;
        v2 = temp;
    }
    
    if (iy2 < iy1) {
        temp = v1;
        v1 = v2;
        v2 = temp;
    }
    
    TRIANGLE_TYPE tri_type;
    
    // 检测是哪种三角形
    if ((int)pv[v0].y == (int)pv[v1].y) {
        // 平顶三角形
        tri_type = TRIANGLE_TYPE::FLAT_TOP_TRIANGLE;
        
        // 从左到右排列顶点
        if (ix1 < ix0) {
            temp = v0;
            v0 = v1;
            v1 = temp;
        }
    }//end if
    else{
        if ((int)pv[v1].y == (int)pv[v2].y) {
            // 平底三角形
            tri_type = TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE;
            
            //从左到右
            if (ix1 < ix2) {
                temp = v1;
                v1 = v2;
                v2 = temp;
            }
        }// end if
        else{
            // 判断三角形是左边长还是右边长
            // 插值计算
            float interx = pv[v0].x + (pv[v1].y - pv[v0].y)*(pv[v2].x - pv[v0].x)/(pv[v2].y - pv[v0].y);
            if (interx > pv[v1].x) {
                tri_type = TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE;
            }else{
                tri_type = TRIANGLE_TYPE::LEFT_LONG_TRIANGLE;
            }
        }// end else
    }// end else
    
    float x0 = pv[v0].x;
    float y0 = pv[v0].y;
    
    float x1 = pv[v1].x;
    float y1 = pv[v1].y;
    
    float x2 = pv[v2].x;
    float y2 = pv[v2].y;
    
    // 提取顶点,准备进行光栅化
    if (tri_type == TRIANGLE_TYPE::FLAT_TOP_TRIANGLE) {
        // 直接绘制
        float xs = x0,xe = x1;
        float dy = y2 - y0;
        float dxdyl = (x2 - x0)/dy;
        float dxdyr = (x2 - x1)/dy;
        
        int endY = y2;
        for (int i = y0; i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdyl;
            xe += dxdyr;
        }
    }else if(tri_type == TRIANGLE_TYPE::FLAT_BOTTOM_TRIANGLE){
        float xs = x0,xe = x0;
        float dy = y1 - y0;
        float dxdyl = (x1-x0)/dy;
        float dxdyr = (x2-x0)/dy;
        
        int endY = y1;
        for (int i = y0; i < endY;++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdyl;
            xe += dxdyr;
        }
    }
    
    else if (tri_type == TRIANGLE_TYPE::RIGHT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        float xs = x0,xe = x0;
        
        //分两段绘制
        int endY = y1;
        for (int i = y0; i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdy10;
            xe += dxdy20;
        }
        
        endY = y2;
        xs = x1;
        for (int i = y1; i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdy21;
            xe += dxdy20;
        }
    }else if (tri_type == TRIANGLE_TYPE::LEFT_LONG_TRIANGLE){
        
        float dy10 = y1 - y0;
        float dxdy10 = (x1 - x0)/dy10;
        
        float dy20 = y2 - y0;
        float dxdy20 = (x2 - x0)/dy20;
        
        float dy21 = y2 - y1;
        float dxdy21 = (x2 - x1)/dy21;
        
        float xs = x0,xe = x0;
        int endY = y1;
        for (int i = y0; i < endY; ++i) {
            DrawLine(xs, i, xe, i, color);
            xs += dxdy20;
            xe += dxdy10;
        }
        
        endY = y2;
        xe = x1;
        for (int i = y1;i < endY; ++i) {
            DrawLine(xs,i,xe,i,color);
            xs += dxdy20;
            xe += dxdy21;
        }
    }
    else{
        printf("DrawTriangle maybe some error......\n");
    }
    
}
#endif
