//
//  RenderLine.cpp
//  SoftEngineV4.0
//
//  Created by user on 14-12-24.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "RenderLine.h"


int RenderClipLine(int &x1,int &y1,int &x2, int &y2)
{
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

void RenderLine(int x0,int y0,int x1,int y1,Uint32 color){
    
    if (RenderClipLine(x0, y0, x1, y1) == 0) {
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