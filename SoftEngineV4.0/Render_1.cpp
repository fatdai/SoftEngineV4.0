//
//  Render.cpp
//  Soft_Engine_V3.0
//
//  Created by user on 14-12-7.
//  Copyright (c) 2014年 user. All rights reserved.
//

#include "Render_1.h"
#include "Texture2.h"

//void swapff(float* a,float* b,float* temp){
//    *temp = *a;
//    *a = *b;
//    *b = *temp;
//}


// fixed point mathematics constants
#define FIXP16_SHIFT     16
#define FIXP16_MAG       65536
#define FIXP16_DP_MASK   0x0000ffff
#define FIXP16_WP_MASK   0xffff0000
#define FIXP16_ROUND_UP  0x00008000


int clip_line(int &x1,int &y1,int &x2, int &y2){
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
    
    int max_clip_y = kWINDOW_HEIGHT-1;
    int min_clip_y = 0;
    int min_clip_x = 0;
    int max_clip_x = kWINDOW_WIDTH-1;
    
    // determine codes for p1 and p2
    if (y1 < min_clip_y)
        p1_code|=CLIP_CODE_N;
    else
        if (y1 > max_clip_y)
            p1_code|=CLIP_CODE_S;
    
    if (x1 < min_clip_x)
        p1_code|=CLIP_CODE_W;
    else
        if (x1 > max_clip_x)
            p1_code|=CLIP_CODE_E;
    
    if (y2 < min_clip_y)
        p2_code|=CLIP_CODE_N;
    else
        if (y2 > max_clip_y)
            p2_code|=CLIP_CODE_S;
    
    if (x2 < min_clip_x)
        p2_code|=CLIP_CODE_W;
    else
        if (x2 > max_clip_x)
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
            yc1 = min_clip_y;
            xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);
        } break;
        case CLIP_CODE_S:
        {
            yc1 = max_clip_y;
            xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);
        } break;
            
        case CLIP_CODE_W:
        {
            xc1 = min_clip_x;
            yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);
        } break;
            
        case CLIP_CODE_E:
        {
            xc1 = max_clip_x;
            yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
        } break;
            
            // these cases are more complex, must compute 2 intersections
        case CLIP_CODE_NE:
        {
            // north hline intersection
            yc1 = min_clip_y;
            xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);
            
            // test if intersection is valid, of so then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
            {
				// east vline intersection
				xc1 = max_clip_x;
				yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
            } // end if
            
        } break;
            
        case CLIP_CODE_SE:
        {
            // south hline intersection
            yc1 = max_clip_y;
            xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc1 < min_clip_x || xc1 > max_clip_x)
            {
                // east vline intersection
                xc1 = max_clip_x;
                yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
            } // end if
            
        } break;
            
        case CLIP_CODE_NW:
        {
            // north hline intersection
            yc1 = min_clip_y;
            xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc1 < min_clip_x || xc1 > max_clip_x)
            {
                xc1 = min_clip_x;
                yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);
            } // end if
            
        } break;
            
        case CLIP_CODE_SW:
        {
            // south hline intersection
            yc1 = max_clip_y;
            xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc1 < min_clip_x || xc1 > max_clip_x)
            {
                xc1 = min_clip_x;
                yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);
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
            yc2 = min_clip_y;
            xc2 = x2 + (min_clip_y-y2)*(x1-x2)/(y1-y2);
        } break;
            
        case CLIP_CODE_S:
        {
            yc2 = max_clip_y;
            xc2 = x2 + (max_clip_y-y2)*(x1-x2)/(y1-y2);
        } break;
            
        case CLIP_CODE_W:
        {
            xc2 = min_clip_x;
            yc2 = y2 + (min_clip_x-x2)*(y1-y2)/(x1-x2);
        } break;
            
        case CLIP_CODE_E:
        {
            xc2 = max_clip_x;
            yc2 = y2 + (max_clip_x-x2)*(y1-y2)/(x1-x2);
        } break;
            
            // these cases are more complex, must compute 2 intersections
        case CLIP_CODE_NE:
        {
            // north hline intersection
            yc2 = min_clip_y;
            xc2 = x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2);
            
            // test if intersection is valid, of so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
            {
				// east vline intersection
				xc2 = max_clip_x;
				yc2 = y2 + 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2);
            } // end if
            
        } break;
            
        case CLIP_CODE_SE:
        {
            // south hline intersection
            yc2 = max_clip_y;
            xc2 = x2 + 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc2 < min_clip_x || xc2 > max_clip_x)
            {
                // east vline intersection
                xc2 = max_clip_x;
                yc2 = y2 + 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2);
            } // end if
            
        } break;
            
        case CLIP_CODE_NW:
        {
            // north hline intersection
            yc2 = min_clip_y;
            xc2 = x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc2 < min_clip_x || xc2 > max_clip_x)
            {
                xc2 = min_clip_x;
                yc2 = y2 + 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2);
            } // end if
            
        } break;
            
        case CLIP_CODE_SW:
        {
            // south hline intersection
            yc2 = max_clip_y;
            xc2 = x2 + 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2);
            
            // test if intersection is valid, of so then done, else compute next
            if (xc2 < min_clip_x || xc2 > max_clip_x)
            {
                xc2 = min_clip_x;
                yc2 = y2 + 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2);
            } // end if
            
        } break;
            
        default:break;
            
    } // end switch
    
    // do bounds check
    if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
        (yc1 < min_clip_y) || (yc1 > max_clip_y) ||
        (xc2 < min_clip_x) || (xc2 > max_clip_x) ||
        (yc2 < min_clip_y) || (yc2 > max_clip_y) )
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


int draw_line_v1(int x0, int y0, int x1, int y1, Uint32 color){
    
    if (clip_line(x0, y0,x1,y1)) {
        // this function draws a line from xo,yo to x1,y1 using differential error
        // terms (based on Bresenahams work)
        
        int dx,             // difference in x's
        dy,             // difference in y's
        dx2,            // dx,dy * 2
        dy2,
        x_inc,          // amount in pixel space to move during drawing
        y_inc,          // amount in pixel space to move during drawing
        error,          // the discriminant i.e. error i.e. decision variable
        index;          // used for looping
        
        int pitchWidth = gSurface->pitch >> 2;
        
        // pre-compute first pixel address in video buffer based on 32bit data
        Uint32* vb_start2 = (Uint32 *)gSurface->pixels + x0 + y0 * pitchWidth;
        
        // compute horizontal and vertical deltas
        dx = x1-x0;
        dy = y1-y0;
        
        // test which direction the line is going in i.e. slope angle
        if (dx>=0)
        {
            x_inc = 1;
            
        } // end if line is moving right
        else
        {
            x_inc = -1;
            dx    = -dx;  // need absolute value
            
        } // end else moving left
        
        // test y component of slope
        
        if (dy>=0)
        {
            y_inc = pitchWidth;
        } // end if line is moving down
        else
        {
            y_inc = -pitchWidth;
            dy    = -dy;  // need absolute value
            
        } // end else moving up
        
        // compute (dx,dy) * 2
        dx2 = dx << 1;
        dy2 = dy << 1;
        
        // now based on which delta is greater we can draw the line
        if (dx > dy)
        {
            // initialize error term
            error = dy2 - dx;
            
            // draw the line
            for (index=0; index <= dx; index++)
            {
                // set the pixel
                *vb_start2 = (Uint32)color;
                
                // test if error has overflowed
                if (error >= 0)
                {
                    error-=dx2;
                    
                    // move to next line
                    vb_start2+=y_inc;
                    
                } // end if error overflowed
                
                // adjust the error term
                error+=dy2;
                
                // move to the next pixel
                vb_start2+=x_inc;
                
            } // end for
            
        } // end if |slope| <= 1
        else
        {
            // initialize error term
            error = dx2 - dy;
            
            // draw the line
            for (index=0; index <= dy; index++)
            {
                // set the pixel
                *vb_start2 = (Uint32)color;
                
                // test if error overflowed
                if (error >= 0)
                {
                    error-=dy2;
                    
                    // move to next line
                    vb_start2+=x_inc;
                    
                } // end if error overflowed
                
                // adjust the error term
                error+=dx2;
                
                // move to the next pixel
                vb_start2+=y_inc;
                
            } // end for
            
        } // end else |slope| > 1
        
        // return success
        return(1);
    }
    return (1);
}

int renderClear(){
    SDL_FillRect(gSurface, NULL, gColorBlack);
    return (1);
}

void swapBuffer(){
    SDL_UpdateWindowSurface(gWindow);
}


void draw_fill_rect(const SDL_Rect& rect,Uint32 color){
    SDL_FillRect(gSurface,&rect,color);
}


void draw_top_triangle(int x1,int y1,int x2,int y2,int x3,int y3,Uint32 color){
    // this function draws a triangle that has a flat top
    
    float dx_right,    // the dx/dy ratio of the right edge of line
    dx_left,     // the dx/dy ratio of the left edge of line
    xs,xe,       // the starting and ending points of the edges
    height;      // the height of the triangle
    
    int temp_x;        // used during sorting as temps
    
    // test order of x1 and x2
    if (x2 < x1)
    {
        temp_x = x2;
        x2     = x1;
        x1     = temp_x;
    } // end if swap
    
    // compute delta's
    height = y3-y1;
    
    dx_left  = (x3-x1)/height;
    dx_right = (x3-x2)/height;
    
    // set starting points
    xs = (float)x1;
    xe = (float)x2; // +(float)0.5;
    
    // draw the triangle
    for (int temp_y=y1; temp_y<=y3; temp_y++)
    {
        // draw the line
        draw_line_v1(xs,temp_y,xe,temp_y,color);
        
        // adjust starting point and ending point
        xs+=dx_left;
        xe+=dx_right;
        
    } // end for
    
}

void draw_bottom_triangle(int x1,int y1,int x2,int y2,int x3,int y3,Uint32 color){
    // this function draws a triangle that has a flat bottom
    
    float dx_right,    // the dx/dy ratio of the right edge of line
    dx_left,     // the dx/dy ratio of the left edge of line
    xs,xe,       // the starting and ending points of the edges
    height;      // the height of the triangle
    
    int temp_x;        // used during sorting as temps
    
    // test order of x1 and x2
    if (x3 < x2)
    {
        temp_x = x2;
        x2     = x3;
        x3     = temp_x;
    } // end if swap
    
    // compute delta's
    height = y3-y1;
    
    dx_left  = (x2-x1)/height;
    dx_right = (x3-x1)/height;
    
    // set starting points
    xs = (float)x1;
    xe = (float)x1; // +(float)0.5;
    
    // draw the triangle
    for (int temp_y = y1; temp_y <= y3; ++temp_y)
    {
        // draw the line
        draw_line_v1(xs,temp_y,xe,temp_y,color);
        
        // adjust starting point and ending point
        xs+=dx_left;
        xe+=dx_right;
        
    } // end for
}

void draw_fill_triangle_v1(int x1,int y1,int x2,int y2,int x3,int y3,Uint32 color){
    int temp_x, // used for sorting
    temp_y,
    new_x;
    
    // test for h lines and v lines
    if ((x1==x2 && x2==x3)  ||  (y1==y2 && y2==y3)){
        return;
    }
    
    // sort p1,p2,p3 in ascending y order
    if (y2<y1)
    {
        temp_x = x2;
        temp_y = y2;
        x2     = x1;
        y2     = y1;
        x1     = temp_x;
        y1     = temp_y;
    } // end if
    
    // now we know that p1 and p2 are in order
    if (y3<y1)
    {
        temp_x = x3;
        temp_y = y3;
        x3     = x1;
        y3     = y1;
        x1     = temp_x;
        y1     = temp_y;
    } // end if
    
    // finally test y3 against y2
    if (y3<y2)
    {
        temp_x = x3;
        temp_y = y3;
        x3     = x2;
        y3     = y2;
        x2     = temp_x;
        y2     = temp_y;
        
    } // end if
    
    // test if top of triangle is flat
    if (y1==y2)
    {
        draw_top_triangle(x1,y1,x2,y2,x3,y3,color);
    } // end if
    else
        if (y2==y3)
        {
            draw_bottom_triangle(x1,y1,x2,y2,x3,y3,color);
        } // end if bottom is flat
        else
        {
            // general triangle that's needs to be broken up along long edge
            new_x = x1 + (int)(0.5+(float)(y2-y1)*(float)(x3-x1)/(float)(y3-y1));
            
            // draw each sub-triangle
            draw_bottom_triangle(x1,y1,new_x,y2,x2,y2,color);
            draw_top_triangle(x2,y2,new_x,y2,x3,y3,color);
            
        } // end else
}


void draw_texture_triangle(Face* face){
    
    // this function draws a textured triangle in 16-bit mode
    
    int v0=0,
    v1=1,
	v2=2,
	temp=0,
	tri_type = TRI_TYPE_NONE,
	irestart = INTERP_LHS;
    
    int dx,dy,dyl,dyr,      // general deltas
//    u,v,
    du,dv,
    xi,yi,              // the current interpolated x,y
	ui,vi,              // the current interpolated u,v
//	index_x,index_y,    // looping vars
//	x,y,                // hold general x,y
	xstart,
	xend,
	ystart,
	yrestart,
	yend,
	xl,
	dxdyl,
	xr,
	dxdyr,
	dudyl,
	ul,
	dvdyl,
	vl,
	dudyr,
	ur,
	dvdyr,
	vr;
    
    int x0,y0,tu0,tv0,    // cached vertices
	x1,y1,tu1,tv1,
	x2,y2,tu2,tv2;
    
    Uint32 *screen_ptr  = NULL,
 //   *screen_line = NULL,
    *textmap     = NULL,
    *dest_buffer = (Uint32 *)gSurface->pixels;

    
    // extract texture map
    textmap = (Uint32 *)face->mati.texture->surface->pixels;
    
    // extract base 2 of texture width
 //   int texture_shift2 = log2f(face->texture->w);
    int texture_pitch = face->mati.texture->surface->pitch >> 2;
    
    // adjust memory pitch to words, divide by 2
    int mem_pitch = gSurface->pitch >> 2;
    
    int min_clip_y = 0;
    int max_clip_y = kWINDOW_HEIGHT-1;
    int min_clip_x = 0;
    int max_clip_x = kWINDOW_WIDTH-1;
    
    // first trivial clipping rejection tests
    if (((face->vlist_trans[0].y < min_clip_y)  &&
         (face->vlist_trans[1].y < min_clip_y)  &&
         (face->vlist_trans[2].y < min_clip_y)) ||
        
        ((face->vlist_trans[0].y > max_clip_y)  &&
         (face->vlist_trans[1].y > max_clip_y)  &&
         (face->vlist_trans[2].y > max_clip_y)) ||
        
        ((face->vlist_trans[0].x < min_clip_x)  &&
         (face->vlist_trans[1].x < min_clip_x)  &&
         (face->vlist_trans[2].x < min_clip_x)) ||
        
        ((face->vlist_trans[0].x > max_clip_x)  &&
         (face->vlist_trans[1].x > max_clip_x)  &&
         (face->vlist_trans[2].x > max_clip_x)))
        return;
    
    // degenerate triangle
    if ( ((face->vlist_trans[0].x==face->vlist_trans[1].x) && (face->vlist_trans[1].x==face->vlist_trans[2].x)) ||
        ((face->vlist_trans[0].y==face->vlist_trans[1].y) && (face->vlist_trans[1].y==face->vlist_trans[2].y)))
        return;
    
    // sort vertices
    if (face->vlist_trans[v1].y < face->vlist_trans[v0].y)
	{SWAP(v0,v1,temp);}
    
    if (face->vlist_trans[v2].y < face->vlist_trans[v0].y)
	{SWAP(v0,v2,temp);}
    
    if (face->vlist_trans[v2].y < face->vlist_trans[v1].y)
	{SWAP(v1,v2,temp);}
    
    // now test for trivial flat sided cases
    if (face->vlist_trans[v0].y==face->vlist_trans[v1].y)
	{
        // set triangle type
        tri_type = TRI_TYPE_FLAT_TOP;
        
        // sort vertices left to right
        if (face->vlist_trans[v1].x < face->vlist_trans[v0].x)
		{SWAP(v0,v1,temp);}
        
	} // end if
    else
        // now test for trivial flat sided cases
        if (face->vlist_trans[v1].y==face->vlist_trans[v2].y)
        {
            // set triangle type
            tri_type = TRI_TYPE_FLAT_BOTTOM;
            
            // sort vertices left to right
            if (face->vlist_trans[v2].x < face->vlist_trans[v1].x)
            {SWAP(v1,v2,temp);}
            
        } // end if
        else
        {
            // must be a general triangle
            tri_type = TRI_TYPE_GENERAL;
            
        } // end else
    
    // extract vertices for processing, now that we have order
    x0  = (int)(face->vlist_trans[v0].x+0.5);
    y0  = (int)(face->vlist_trans[v0].y+0.5);
    tu0 = (int)(face->vlist_trans[v0].tu);
    tv0 = (int)(face->vlist_trans[v0].tv);
    
    x1  = (int)(face->vlist_trans[v1].x+0.5);
    y1  = (int)(face->vlist_trans[v1].y+0.5);
    tu1 = (int)(face->vlist_trans[v1].tu);
    tv1 = (int)(face->vlist_trans[v1].tv);
    
    x2  = (int)(face->vlist_trans[v2].x+0.5);
    y2  = (int)(face->vlist_trans[v2].y+0.5);
    tu2 = (int)(face->vlist_trans[v2].tu);
    tv2 = (int)(face->vlist_trans[v2].tv);
    
    // set interpolation restart value
    yrestart = y1;
    
    // what kind of triangle
    if (tri_type & TRI_TYPE_FLAT_MASK)
	{
        
        if (tri_type == TRI_TYPE_FLAT_TOP)
        {
            // compute all deltas
            dy = (y2 - y0);
            
            if (dy == 0) {
                dy = 1;
            }
            
            dxdyl = ((x2 - x0)   << FIXP16_SHIFT)/dy;
            dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
            dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
            
            dxdyr = ((x2 - x1)   << FIXP16_SHIFT)/dy;
            dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;
            dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;
            
            // test for y clipping
            if (y0 < min_clip_y)
            {
                // compute overclip
                dy = (min_clip_y - y0);
                
                // computer new LHS starting values
                xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
                ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
                vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
                
                // compute new RHS starting values
                xr = dxdyr*dy + (x1  << FIXP16_SHIFT);
                ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
                vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
                
                // compute new starting y
                ystart = min_clip_y;
                
            } // end if
            else
            {
                // no clipping
                
                // set starting values
                xl = (x0 << FIXP16_SHIFT);
                xr = (x1 << FIXP16_SHIFT);
                
                ul = (tu0 << FIXP16_SHIFT);
                vl = (tv0 << FIXP16_SHIFT);
                
                ur = (tu1 << FIXP16_SHIFT);
                vr = (tv1 << FIXP16_SHIFT);
                
                // set starting y
                ystart = y0;
                
            } // end else
            
        } // end if flat top
        else
        {
            // must be flat bottom
            
            // compute all deltas
            dy = (y1 - y0);
            
            if (dy == 0) {
                dy = 1;
            }
            
            dxdyl = ((x1 - x0)   << FIXP16_SHIFT)/dy;
            dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;
            dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;
            
            dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dy;
            dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
            dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
            
            // test for y clipping
            if (y0 < min_clip_y)
            {
                // compute overclip
                dy = (min_clip_y - y0);
                
                // computer new LHS starting values
                xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
                ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
                vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
                
                // compute new RHS starting values
                xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
                ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
                vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
                
                // compute new starting y
                ystart = min_clip_y;
                
            } // end if
            else
            {
                // no clipping
                
                // set starting values
                xl = (x0 << FIXP16_SHIFT);
                xr = (x0 << FIXP16_SHIFT);
                
                ul = (tu0 << FIXP16_SHIFT);
                vl = (tv0 << FIXP16_SHIFT);
                
                ur = (tu0 << FIXP16_SHIFT);
                vr = (tv0 << FIXP16_SHIFT);
                
                // set starting y
                ystart = y0;
                
            } // end else
            
        } // end else flat bottom
        
        // test for bottom clip, always
        if ((yend = y2) > max_clip_y)
            yend = max_clip_y;
        
        // test for horizontal clipping
        if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
            (x1 < min_clip_x) || (x1 > max_clip_x) ||
            (x2 < min_clip_x) || (x2 > max_clip_x))
        {
            // clip version
            
            // point screen ptr to starting line
            screen_ptr = dest_buffer + (ystart * mem_pitch);
            
            for (yi = ystart; yi<=yend; yi++)
            {
                // compute span endpoints
                xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                
                // compute starting points for u,v interpolants
                ui = ul + FIXP16_ROUND_UP;
                vi = vl + FIXP16_ROUND_UP;
                
                // compute u,v interpolants
                if ((dx = (xend - xstart))>0)
                {
                    du = (ur - ul)/dx;
                    dv = (vr - vl)/dx;
                } // end if
                else
                {
                    du = (ur - ul);
                    dv = (vr - vl);
                } // end else
                
                ///////////////////////////////////////////////////////////////////////
                
                // test for x clipping, LHS
                if (xstart < min_clip_x)
                {
                    // compute x overlap
                    dx = min_clip_x - xstart;
                    
                    // slide interpolants over
                    ui+=dx*du;
                    vi+=dx*dv;
                    
                    // reset vars
                    xstart = min_clip_x;
                    
                } // end if
                
                // test for x clipping RHS
                if (xend > max_clip_x)
                    xend = max_clip_x;
                
                ///////////////////////////////////////////////////////////////////////
                
                // draw span
                for (xi=xstart; xi<=xend; xi++)
                {
                    // write textel
                    screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * texture_pitch)];
                    
                    // interpolate u,v
                    ui+=du;
                    vi+=dv;
                } // end for xi
                
                // interpolate u,v,x along right and left edge
                xl+=dxdyl;
                ul+=dudyl;
                vl+=dvdyl;
                
                xr+=dxdyr;
                ur+=dudyr;
                vr+=dvdyr;
                
                // advance screen ptr
                screen_ptr+=mem_pitch;
                
            } // end for y
            
        } // end if clip
        else
        {
            // non-clip version
            
            // point screen ptr to starting line
            screen_ptr = dest_buffer + (ystart * mem_pitch);
            
            for (yi = ystart; yi<=yend; yi++)
            {
                // compute span endpoints
                xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                
                // compute starting points for u,v interpolants
                ui = ul + FIXP16_ROUND_UP;
                vi = vl + FIXP16_ROUND_UP;
                
                // compute u,v interpolants
                if ((dx = (xend - xstart))>0)
                {
                    du = (ur - ul)/dx;
                    dv = (vr - vl)/dx;
                } // end if
                else
                {
                    du = (ur - ul);
                    dv = (vr - vl);
                } // end else
                
                // draw span
                for (xi=xstart; xi<=xend; xi++)
                {
                    // write textel
                    screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) +  ((vi >> FIXP16_SHIFT) * texture_pitch)];
                    
                    // interpolate u,v
                    ui+=du;
                    vi+=dv;
                } // end for xi
                
                // interpolate u,v,x along right and left edge
                xl+=dxdyl;
                ul+=dudyl;
                vl+=dvdyl;
                
                xr+=dxdyr;
                ur+=dudyr;
                vr+=dvdyr;
                
                // advance screen ptr
                screen_ptr+=mem_pitch;
                
            } // end for y
            
        } // end if non-clipped
        
	} // end if
    else
        if (tri_type==TRI_TYPE_GENERAL)
        {
            
            // first test for bottom clip, always
            if ((yend = y2) > max_clip_y)
                yend = max_clip_y;
            
            // pre-test y clipping status
            if (y1 < min_clip_y)
            {
                // compute all deltas
                // LHS
                dyl = (y2 - y1);
                
                if (dyl == 0) {
                    dyl = 1;
                }
                
                dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
                dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
                dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
                
                // RHS
                dyr = (y2 - y0);
                if (dyr == 0) {
                    dyr = 1;
                }
                
                dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
                dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
                dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
                
                // compute overclip
                dyr = (min_clip_y - y0);
                dyl = (min_clip_y - y1);
                
                // computer new LHS starting values
                xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
                ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
                vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
                
                // compute new RHS starting values
                xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
                ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
                vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
                
                // compute new starting y
                ystart = min_clip_y;
                
                // test if we need swap to keep rendering left to right
                if (dxdyr > dxdyl)
                {
                    SWAP(dxdyl,dxdyr,temp);
                    SWAP(dudyl,dudyr,temp);
                    SWAP(dvdyl,dvdyr,temp);
                    SWAP(xl,xr,temp);
                    SWAP(ul,ur,temp);
                    SWAP(vl,vr,temp);
                    SWAP(x1,x2,temp);
                    SWAP(y1,y2,temp);
                    SWAP(tu1,tu2,temp);
                    SWAP(tv1,tv2,temp);
                    
                    // set interpolation restart
                    irestart = INTERP_RHS;
                    
                } // end if
                
            } // end if
            else
                if (y0 < min_clip_y)
                {
                    // compute all deltas
                    // LHS
                    dyl = (y1 - y0);
                    
                    dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
                    dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
                    dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
                    
                    // RHS
                    dyr = (y2 - y0);
                    
                    dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
                    dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
                    dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
                    
                    // compute overclip
                    dy = (min_clip_y - y0);
                    
                    // computer new LHS starting values
                    xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
                    ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
                    vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
                    
                    // compute new RHS starting values
                    xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
                    ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
                    vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
                    
                    // compute new starting y
                    ystart = min_clip_y;
                    
                    // test if we need swap to keep rendering left to right
                    if (dxdyr < dxdyl)
                    {
                        SWAP(dxdyl,dxdyr,temp);
                        SWAP(dudyl,dudyr,temp);
                        SWAP(dvdyl,dvdyr,temp);
                        SWAP(xl,xr,temp);
                        SWAP(ul,ur,temp);
                        SWAP(vl,vr,temp);
                        SWAP(x1,x2,temp);
                        SWAP(y1,y2,temp);
                        SWAP(tu1,tu2,temp);
                        SWAP(tv1,tv2,temp);
                        
                        // set interpolation restart
                        irestart = INTERP_RHS;
                        
                    } // end if
                    
                } // end if
                else
                {
                    // no initial y clipping
                    
                    // compute all deltas
                    // LHS
                    dyl = (y1 - y0);
                    
                    // added by dai
                    if (dyl == 0) {
                        dyl = 1;
                    }
                    
                    dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
                    dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
                    dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
                    
                    // RHS
                    dyr = (y2 - y0);	
                    if (dyr == 0) {
                        dyr = 1;
                    }
                    
                    dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
                    dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
                    dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
                    
                    // no clipping y
                    
                    // set starting values
                    xl = (x0 << FIXP16_SHIFT);
                    xr = (x0 << FIXP16_SHIFT);
                    
                    ul = (tu0 << FIXP16_SHIFT);
                    vl = (tv0 << FIXP16_SHIFT);
                    
                    ur = (tu0 << FIXP16_SHIFT);
                    vr = (tv0 << FIXP16_SHIFT);
                    
                    // set starting y
                    ystart = y0;
                    
                    // test if we need swap to keep rendering left to right
                    if (dxdyr < dxdyl)
                    {
                        SWAP(dxdyl,dxdyr,temp);
                        SWAP(dudyl,dudyr,temp);
                        SWAP(dvdyl,dvdyr,temp);
                        SWAP(xl,xr,temp);
                        SWAP(ul,ur,temp);
                        SWAP(vl,vr,temp);
                        SWAP(x1,x2,temp);
                        SWAP(y1,y2,temp);
                        SWAP(tu1,tu2,temp);
                        SWAP(tv1,tv2,temp);
                        
                        // set interpolation restart
                        irestart = INTERP_RHS;
                        
                    } // end if
                    
                } // end else
            
            
            // test for horizontal clipping
            if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
                (x1 < min_clip_x) || (x1 > max_clip_x) ||
                (x2 < min_clip_x) || (x2 > max_clip_x))
            {
                // clip version
                // x clipping	
                
                // point screen ptr to starting line
                screen_ptr = dest_buffer + (ystart * mem_pitch);
                
                for (yi = ystart; yi<=yend; yi++)
                {
                    // compute span endpoints
                    xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                    xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                    
                    // compute starting points for u,v interpolants
                    ui = ul + FIXP16_ROUND_UP;
                    vi = vl + FIXP16_ROUND_UP;
                    
                    // compute u,v interpolants
                    if ((dx = (xend - xstart))>0)
                    {
                        du = (ur - ul)/dx;
                        dv = (vr - vl)/dx;
                    } // end if
                    else
                    {
                        du = (ur - ul);
                        dv = (vr - vl);
                    } // end else
                    
                    ///////////////////////////////////////////////////////////////////////
                    
                    // test for x clipping, LHS
                    if (xstart < min_clip_x)
                    {
                        // compute x overlap
                        dx = min_clip_x - xstart;
                        
                        // slide interpolants over
                        ui+=dx*du;
                        vi+=dx*dv;
                        
                        // set x to left clip edge
                        xstart = min_clip_x;
                        
                    } // end if
                    
                    // test for x clipping RHS
                    if (xend > max_clip_x)
                        xend = max_clip_x;
                    
                    ///////////////////////////////////////////////////////////////////////
                    
                    // draw span
                    for (xi=xstart; xi<=xend; xi++)
                    {
                        // write textel
                        screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * texture_pitch)];
                        
                        // interpolate u,v
                        ui+=du;
                        vi+=dv;
                    } // end for xi
                    
                    // interpolate u,v,x along right and left edge
                    xl+=dxdyl;
                    ul+=dudyl;
                    vl+=dvdyl;
                    
                    xr+=dxdyr;
                    ur+=dudyr;
                    vr+=dvdyr;
                    
                    // advance screen ptr
                    screen_ptr+=mem_pitch;
                    
                    // test for yi hitting second region, if so change interpolant
                    if (yi==yrestart)
                    {
                        
                        // test interpolation side change flag
                        
                        if (irestart == INTERP_LHS)
                        {
                            // LHS
                            dyl = (y2 - y1);	
                            
                            dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
                            dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
                            dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
                            
                            // set starting values
                            xl = (x1  << FIXP16_SHIFT);
                            ul = (tu1 << FIXP16_SHIFT);
                            vl = (tv1 << FIXP16_SHIFT);
                            
                            // interpolate down on LHS to even up
                            xl+=dxdyl;
                            ul+=dudyl;
                            vl+=dvdyl;
                        } // end if
                        else
                        {
                            // RHS
                            dyr = (y1 - y2);
                            
                            // add by dai
                            if (dyr == 0) {
                                dyr = 1;
                            }
                            
                            dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
                            dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
                            dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
                            
                            // set starting values
                            xr = (x2  << FIXP16_SHIFT);
                            ur = (tu2 << FIXP16_SHIFT);
                            vr = (tv2 << FIXP16_SHIFT);
                            
                            // interpolate down on RHS to even up
                            xr+=dxdyr;
                            ur+=dudyr;
                            vr+=dvdyr;
                            
                        } // end else
                        
                        
                    } // end if
                    
                } // end for y
                
            } // end if
            else
            {
                // no x clipping
                // point screen ptr to starting line
                screen_ptr = dest_buffer + (ystart * mem_pitch);
                
                for (yi = ystart; yi<=yend; yi++)
                {
                    // compute span endpoints
                    xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                    xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                    
                    // compute starting points for u,v interpolants
                    ui = ul + FIXP16_ROUND_UP;
                    vi = vl + FIXP16_ROUND_UP;
                    
                    // compute u,v interpolants
                    if ((dx = (xend - xstart))>0)
                        
                    {
                        du = (ur - ul)/dx;
                        dv = (vr - vl)/dx;
                    } // end if
                    else
                    {
                        du = (ur - ul);
                        dv = (vr - vl);
                    } // end else
                    
                    // draw span
                    for (xi=xstart; xi<=xend; xi++)
                    {
                        // write textel
                        screen_ptr[xi] = textmap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) * texture_pitch)];
                        
                        // interpolate u,v
                        ui+=du;
                        vi+=dv;
                        
                    } // end for xi
                    
                    // interpolate u,v,x along right and left edge
                    xl+=dxdyl;
                    ul+=dudyl;
                    vl+=dvdyl;
                    
                    xr+=dxdyr;
                    ur+=dudyr;
                    vr+=dvdyr;
                    
                    // advance screen ptr
                    screen_ptr+=mem_pitch;
                    
                    // test for yi hitting second region, if so change interpolant
                    if (yi==yrestart)
                    {
                        // test interpolation side change flag
                        
                        if (irestart == INTERP_LHS)
                        {
                            // LHS
                            dyl = (y2 - y1);	
                            
                            // add by dai
                            if (dyl == 0) {
                                dyl = 1;
                            }
                            
                            dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
                            dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
                            dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
                            
                            // set starting values
                            xl = (x1  << FIXP16_SHIFT);
                            ul = (tu1 << FIXP16_SHIFT);
                            vl = (tv1 << FIXP16_SHIFT);
                            
                            // interpolate down on LHS to even up
                            xl+=dxdyl;
                            ul+=dudyl;
                            vl+=dvdyl;
                        } // end if
                        else
                        {
                            // RHS
                            dyr = (y1 - y2);
                            
                            // add by dai
                            if (dyr == 0) {
                                dyr = 1;
                            }
                            
                            dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
                            dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
                            dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
                            
                            // set starting values
                            xr = (x2  << FIXP16_SHIFT);
                            ur = (tu2 << FIXP16_SHIFT);
                            vr = (tv2 << FIXP16_SHIFT);
                            
                            // interpolate down on RHS to even up
                            xr+=dxdyr;
                            ur+=dudyr;
                            vr+=dvdyr;
                            
                        } // end else
                        
                    } // end if
                    
                } // end for y
                
            } // end else	
            
        } // end if
}



//////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void draw_gouraud_triangle(Face* face)       // bytes per line, 320, 640 etc.
{
    // this function draws a gouraud shaded polygon, based on the affine texture mapper, instead
    // of interpolating the texture coordinates, we simply interpolate the (R,G,B) values across
    // the polygons, I simply needed at another interpolant, I have mapped u->red, v->green, w->blue
    
    int v0=0,
    v1=1,
	v2=2,
	temp=0,
	tri_type = TRI_TYPE_NONE,
	irestart = INTERP_LHS;
    
    int dx,dy,dyl,dyr,      // general deltas
 //   u,v,w,
    du,dv,dw,
    xi,yi,              // the current interpolated x,y
	ui,vi,wi,           // the current interpolated u,v
//	index_x,index_y,    // looping vars
//	x,y,                // hold general x,y
	xstart,
	xend,
	ystart,
	yrestart,
	yend,
	xl,
	dxdyl,
	xr,
	dxdyr,
	dudyl,
	ul,
	dvdyl,
	vl,
	dwdyl,
	wl,
	dudyr,
	ur,
	dvdyr,
	vr,
	dwdyr,
	wr;
    
    int x0,y0,tu0,tv0,tw0,    // cached vertices
	x1,y1,tu1,tv1,tw1,
	x2,y2,tu2,tv2,tw2;
    
    Uint8 r_base0, g_base0, b_base0,
    r_base1, g_base1, b_base1,
    r_base2, g_base2, b_base2;
    
    Uint32 *screen_ptr  = NULL;
//    *screen_line = NULL,
//    *textmap     = NULL,
    Uint32 *dest_buffer = (Uint32 *)gSurface->pixels;
    
    // adjust memory pitch to words, divide by 2
    int mem_pitch = gSurface->pitch >> 2;
    
    int min_clip_y = 0;
    int max_clip_y = kWINDOW_HEIGHT-1;
    int min_clip_x = 0;
    int max_clip_x = kWINDOW_WIDTH-1;
    
    // first trivial clipping rejection tests
    if (((face->vlist_trans[0].y < min_clip_y)  &&
         (face->vlist_trans[1].y < min_clip_y)  &&
         (face->vlist_trans[2].y < min_clip_y)) ||
        
        ((face->vlist_trans[0].y > max_clip_y)  &&
         (face->vlist_trans[1].y > max_clip_y)  &&
         (face->vlist_trans[2].y > max_clip_y)) ||
        
        ((face->vlist_trans[0].x < min_clip_x)  &&
         (face->vlist_trans[1].x < min_clip_x)  &&
         (face->vlist_trans[2].x < min_clip_x)) ||
        
        ((face->vlist_trans[0].x > max_clip_x)  &&
         (face->vlist_trans[1].x > max_clip_x)  &&
         (face->vlist_trans[2].x > max_clip_x)))
        return;
    
    // degenerate triangle
    if ( ((face->vlist_trans[0].x==face->vlist_trans[1].x) && (face->vlist_trans[1].x==face->vlist_trans[2].x)) ||
        ((face->vlist_trans[0].y==face->vlist_trans[1].y) && (face->vlist_trans[1].y==face->vlist_trans[2].y)))
        return;
    
    
    // sort vertices
    if (face->vlist_trans[v1].y < face->vlist_trans[v0].y)
	{SWAP(v0,v1,temp);}
    
    if (face->vlist_trans[v2].y < face->vlist_trans[v0].y)
	{SWAP(v0,v2,temp);}
    
    if (face->vlist_trans[v2].y < face->vlist_trans[v1].y)
	{SWAP(v1,v2,temp);}
    
    // now test for trivial flat sided cases
    if (face->vlist_trans[v0].y==face->vlist_trans[v1].y)
	{
        // set triangle type
        tri_type = TRI_TYPE_FLAT_TOP;
        
        // sort vertices left to right
        if (face->vlist_trans[v1].x < face->vlist_trans[v0].x)
		{SWAP(v0,v1,temp);}
        
	} // end if
    else
        // now test for trivial flat sided cases
        if (face->vlist_trans[v1].y==face->vlist_trans[v2].y)
        {
            // set triangle type
            tri_type = TRI_TYPE_FLAT_BOTTOM;
            
            // sort vertices left to right
            if (face->vlist_trans[v2].x < face->vlist_trans[v1].x)
            {SWAP(v1,v2,temp);}
            
        } // end if
        else
        {
            // must be a general triangle
            tri_type = TRI_TYPE_GENERAL;
            
        } // end else
    
    // assume 5.6.5 format -- sorry!
    // we can't afford a function call in the inner loops, so we must write
    // two hard coded versions, if we want support for both 5.6.5, and 5.5.5
//    SDL_GetRGB(face->lit_color[v0], gSurface->format,&r_base0, &g_base0, &b_base0);
//    SDL_GetRGB(face->lit_color[v1], gSurface->format,&r_base1, &g_base1, &b_base1);
//    SDL_GetRGB(face->lit_color[v2], gSurface->format,&r_base2, &g_base2, &b_base2);
    
    r_base0 = face->lit_color[v0].r;
    g_base0 = face->lit_color[v0].g;
    b_base0 = face->lit_color[v0].b;
    
    r_base1 = face->lit_color[v1].r;
    g_base1 = face->lit_color[v1].g;
    b_base1 = face->lit_color[v1].b;
    
    r_base2 = face->lit_color[v2].r;
    g_base2 = face->lit_color[v2].g;
    b_base2 = face->lit_color[v2].b;
    
    // extract vertices for processing, now that we have order
    x0  = (int)(face->vlist_trans[v0].x+0.5);
    y0  = (int)(face->vlist_trans[v0].y+0.5);
    
    tu0 = r_base0;
    tv0 = g_base0;
    tw0 = b_base0;
    
    x1  = (int)(face->vlist_trans[v1].x+0.5);
    y1  = (int)(face->vlist_trans[v1].y+0.5);
    
    tu1 = r_base1;
    tv1 = g_base1;
    tw1 = b_base1;
    
    x2  = (int)(face->vlist_trans[v2].x+0.5);
    y2  = (int)(face->vlist_trans[v2].y+0.5);
    
    tu2 = r_base2;
    tv2 = g_base2;
    tw2 = b_base2;
    
    // set interpolation restart value
    yrestart = y1;
    
    // what kind of triangle
    if (tri_type & TRI_TYPE_FLAT_MASK)
	{
        
        if (tri_type == TRI_TYPE_FLAT_TOP)
        {
            // compute all deltas
            dy = (y2 - y0);
            
            dxdyl = ((x2 - x0)   << FIXP16_SHIFT)/dy;
            dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
            dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
            dwdyl = ((tw2 - tw0) << FIXP16_SHIFT)/dy;
            
            dxdyr = ((x2 - x1)   << FIXP16_SHIFT)/dy;
            dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;
            dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;
            dwdyr = ((tw2 - tw1) << FIXP16_SHIFT)/dy;
            
            // test for y clipping
            if (y0 < min_clip_y)
            {
                // compute overclip
                dy = (min_clip_y - y0);
                
                // computer new LHS starting values
                xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
                ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
                vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
                wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
                
                // compute new RHS starting values
                xr = dxdyr*dy + (x1  << FIXP16_SHIFT);
                ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
                vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
                wr = dwdyr*dy + (tw1 << FIXP16_SHIFT);
                
                // compute new starting y
                ystart = min_clip_y;
                
            } // end if
            else
            {
                // no clipping
                
                // set starting values
                xl = (x0 << FIXP16_SHIFT);
                xr = (x1 << FIXP16_SHIFT);
                
                ul = (tu0 << FIXP16_SHIFT);
                vl = (tv0 << FIXP16_SHIFT);
                wl = (tw0 << FIXP16_SHIFT);
                
                ur = (tu1 << FIXP16_SHIFT);
                vr = (tv1 << FIXP16_SHIFT);
                wr = (tw1 << FIXP16_SHIFT);
                
                // set starting y
                ystart = y0;
                
            } // end else
            
        } // end if flat top
        else
        {
            // must be flat bottom
            
            // compute all deltas
            dy = (y1 - y0);
            
            dxdyl = ((x1 - x0)   << FIXP16_SHIFT)/dy;
            dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;
            dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;
            dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dy;
            
            dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dy;
            dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;
            dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;
            dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dy;
            
            // test for y clipping
            if (y0 < min_clip_y)
            {
                // compute overclip
                dy = (min_clip_y - y0);
                
                // computer new LHS starting values
                xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
                ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
                vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
                wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
                
                // compute new RHS starting values
                xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
                ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
                vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
                wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
                
                // compute new starting y
                ystart = min_clip_y;
                
            } // end if
            else
            {
                // no clipping
                
                // set starting values
                xl = (x0 << FIXP16_SHIFT);
                xr = (x0 << FIXP16_SHIFT);
                
                ul = (tu0 << FIXP16_SHIFT);
                vl = (tv0 << FIXP16_SHIFT);
                wl = (tw0 << FIXP16_SHIFT);
                
                ur = (tu0 << FIXP16_SHIFT);
                vr = (tv0 << FIXP16_SHIFT);
                wr = (tw0 << FIXP16_SHIFT);
                
                // set starting y
                ystart = y0;
                
            } // end else
            
        } // end else flat bottom
        
        // test for bottom clip, always
        if ((yend = y2) > max_clip_y)
            yend = max_clip_y;
        
        // test for horizontal clipping
        if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
            (x1 < min_clip_x) || (x1 > max_clip_x) ||
            (x2 < min_clip_x) || (x2 > max_clip_x))
        {
            // clip version
            
            // point screen ptr to starting line
            screen_ptr = dest_buffer + (ystart * mem_pitch);
            
            for (yi = ystart; yi<=yend; yi++)
            {
                // compute span endpoints
                xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                
                // compute starting points for u,v,w interpolants
                ui = ul + FIXP16_ROUND_UP;
                vi = vl + FIXP16_ROUND_UP;
                wi = wl + FIXP16_ROUND_UP;
                
                // compute u,v interpolants
                if ((dx = (xend - xstart))>0)
                {
                    du = (ur - ul)/dx;
                    dv = (vr - vl)/dx;
                    dw = (wr - wl)/dx;
                } // end if
                else
                {
                    du = (ur - ul);
                    dv = (vr - vl);
                    dw = (wr - wl);
                } // end else
                
                ///////////////////////////////////////////////////////////////////////
                
                // test for x clipping, LHS
                if (xstart < min_clip_x)
                {
                    // compute x overlap
                    dx = min_clip_x - xstart;
                    
                    // slide interpolants over
                    ui+=dx*du;
                    vi+=dx*dv;
                    wi+=dx*dw;
                    
                    // reset vars
                    xstart = min_clip_x;
                    
                } // end if
                
                // test for x clipping RHS
                if (xend > max_clip_x)
                    xend = max_clip_x;
                
                ///////////////////////////////////////////////////////////////////////
                
                // draw span
                for (xi=xstart; xi<=xend; xi++)
                {
                    screen_ptr[xi] = SDL_MapRGB(gSurface->format,
                                                (Uint8)(ui >> (FIXP16_SHIFT)),
                                                        (Uint8)(vi >> (FIXP16_SHIFT)),
                                                        (Uint8)(wi >> (FIXP16_SHIFT)));
                    
                    // interpolate u,v
                    ui+=du;
                    vi+=dv;
                    wi+=dw;
                } // end for xi
                
                // interpolate u,v,w,x along right and left edge
                xl+=dxdyl;
                ul+=dudyl;
                vl+=dvdyl;
                wl+=dwdyl;
                
                xr+=dxdyr;
                ur+=dudyr;
                vr+=dvdyr;
                wr+=dwdyr;
                
                // advance screen ptr
                screen_ptr+=mem_pitch;
                
            } // end for y
            
        } // end if clip
        else
        {
            // non-clip version
            
            // point screen ptr to starting line
            screen_ptr = dest_buffer + (ystart * mem_pitch);
            
            for (yi = ystart; yi<=yend; yi++)
            {
                // compute span endpoints
                xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                
                // compute starting points for u,v,w interpolants
                ui = ul + FIXP16_ROUND_UP;
                vi = vl + FIXP16_ROUND_UP;
                wi = wl + FIXP16_ROUND_UP;
                
                // compute u,v interpolants
                if ((dx = (xend - xstart))>0)
                {
                    du = (ur - ul)/dx;
                    dv = (vr - vl)/dx;
                    dw = (wr - wl)/dx;
                } // end if
                else
                {
                    du = (ur - ul);
                    dv = (vr - vl);
                    dw = (wr - wl);
                } // end else
                
                // draw span
                for (xi=xstart; xi<=xend; xi++)
                {
                    // write textel 5.6.5
                    screen_ptr[xi] = SDL_MapRGB(gSurface->format,
                                                (Uint8)(ui >> (FIXP16_SHIFT)),
                                                        (Uint8)(vi >> (FIXP16_SHIFT)),
                                                        (Uint8)(wi >> (FIXP16_SHIFT)));
                    
                    // interpolate u,v
                    ui+=du;
                    vi+=dv;
                    wi+=dw;
                } // end for xi
                
                // interpolate u,v,w,x along right and left edge
                xl+=dxdyl;
                ul+=dudyl;
                vl+=dvdyl;
                wl+=dwdyl;
                
                xr+=dxdyr;
                ur+=dudyr;
                vr+=dvdyr;
                wr+=dwdyr;
                
                // advance screen ptr
                screen_ptr+=mem_pitch;
                
            } // end for y
            
        } // end if non-clipped
        
	} // end if
    else
        if (tri_type==TRI_TYPE_GENERAL)
        {
            
            // first test for bottom clip, always
            if ((yend = y2) > max_clip_y)
                yend = max_clip_y;
            
            // pre-test y clipping status
            if (y1 < min_clip_y)
            {
                // compute all deltas
                // LHS
                dyl = (y2 - y1);
                
                dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
                dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;
                dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
                dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;
                
                // RHS
                dyr = (y2 - y0);
                
                dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
                dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
                dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
                dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
                
                // compute overclip
                dyr = (min_clip_y - y0);
                dyl = (min_clip_y - y1);
                
                // computer new LHS starting values
                xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
                
                ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
                vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
                wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
                
                // compute new RHS starting values
                xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
                
                ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
                vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
                wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
                
                // compute new starting y
                ystart = min_clip_y;
                
                // test if we need swap to keep rendering left to right
                if (dxdyr > dxdyl)
                {
                    SWAP(dxdyl,dxdyr,temp);
                    SWAP(dudyl,dudyr,temp);
                    SWAP(dvdyl,dvdyr,temp);
                    SWAP(dwdyl,dwdyr,temp);
                    SWAP(xl,xr,temp);
                    SWAP(ul,ur,temp);
                    SWAP(vl,vr,temp);
                    SWAP(wl,wr,temp);
                    SWAP(x1,x2,temp);
                    SWAP(y1,y2,temp);
                    SWAP(tu1,tu2,temp);
                    SWAP(tv1,tv2,temp);
                    SWAP(tw1,tw2,temp);
                    
                    // set interpolation restart
                    irestart = INTERP_RHS;
                    
                } // end if
                
            } // end if
            else
                if (y0 < min_clip_y)
                {
                    // compute all deltas
                    // LHS
                    dyl = (y1 - y0);
                    
                    dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
                    dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;
                    dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;
                    dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;
                    
                    // RHS
                    dyr = (y2 - y0);
                    
                    dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
                    dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;
                    dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;
                    dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
                    
                    // compute overclip
                    dy = (min_clip_y - y0);
                    
                    // computer new LHS starting values
                    xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
                    ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
                    vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
                    wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
                    
                    // compute new RHS starting values
                    xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
                    ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
                    vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
                    wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
                    
                    // compute new starting y
                    ystart = min_clip_y;
                    
                    // test if we need swap to keep rendering left to right
                    if (dxdyr < dxdyl)
                    {
                        SWAP(dxdyl,dxdyr,temp);
                        SWAP(dudyl,dudyr,temp);
                        SWAP(dvdyl,dvdyr,temp);
                        SWAP(dwdyl,dwdyr,temp);
                        SWAP(xl,xr,temp);
                        SWAP(ul,ur,temp);
                        SWAP(vl,vr,temp);
                        SWAP(wl,wr,temp);
                        SWAP(x1,x2,temp);
                        SWAP(y1,y2,temp);
                        SWAP(tu1,tu2,temp);
                        SWAP(tv1,tv2,temp);
                        SWAP(tw1,tw2,temp);
                        
                        // set interpolation restart
                        irestart = INTERP_RHS;
                        
                    } // end if
                    
                } // end if
                else
                {
                    // no initial y clipping
                    
                    // compute all deltas
                    // LHS
                    dyl = (y1 - y0);
                    
                    if (dyl == 0) {
                        dyl = 1;
                    }
                    
                    dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
                    dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
                    dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
                    dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
                    
                    // RHS
                    dyr = (y2 - y0);
                    
                    if (dyr == 0) {
                        dyr = 1;
                    }
                    
                    dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
                    dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
                    dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
                    dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
                    
                    // no clipping y
                    
                    // set starting values
                    xl = (x0 << FIXP16_SHIFT);
                    xr = (x0 << FIXP16_SHIFT);
                    
                    ul = (tu0 << FIXP16_SHIFT);
                    vl = (tv0 << FIXP16_SHIFT);
                    wl = (tw0 << FIXP16_SHIFT);
                    
                    ur = (tu0 << FIXP16_SHIFT);
                    vr = (tv0 << FIXP16_SHIFT);
                    wr = (tw0 << FIXP16_SHIFT);
                    
                    // set starting y
                    ystart = y0;
                    
                    // test if we need swap to keep rendering left to right
                    if (dxdyr < dxdyl)
                    {
                        SWAP(dxdyl,dxdyr,temp);
                        SWAP(dudyl,dudyr,temp);
                        SWAP(dvdyl,dvdyr,temp);
                        SWAP(dwdyl,dwdyr,temp);
                        SWAP(xl,xr,temp);
                        SWAP(ul,ur,temp);
                        SWAP(vl,vr,temp);
                        SWAP(wl,wr,temp);
                        SWAP(x1,x2,temp);
                        SWAP(y1,y2,temp);
                        SWAP(tu1,tu2,temp);
                        SWAP(tv1,tv2,temp);
                        SWAP(tw1,tw2,temp);
                        
                        // set interpolation restart
                        irestart = INTERP_RHS;
                        
                    } // end if
                    
                } // end else
            
            // test for horizontal clipping
            if ((x0 < min_clip_x) || (x0 > max_clip_x) ||
                (x1 < min_clip_x) || (x1 > max_clip_x) ||
                (x2 < min_clip_x) || (x2 > max_clip_x))
            {
                // clip version
                // x clipping	
                
                // point screen ptr to starting line
                screen_ptr = dest_buffer + (ystart * mem_pitch);
                
                for (yi = ystart; yi<=yend; yi++)
                {
                    // compute span endpoints
                    xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                    xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                    
                    // compute starting points for u,v,w interpolants
                    ui = ul + FIXP16_ROUND_UP;
                    vi = vl + FIXP16_ROUND_UP;
                    wi = wl + FIXP16_ROUND_UP;
                    
                    // compute u,v interpolants
                    if ((dx = (xend - xstart))>0)
                    {
                        du = (ur - ul)/dx;
                        dv = (vr - vl)/dx;
                        dw = (wr - wl)/dx;
                    } // end if
                    else
                    {
                        du = (ur - ul);
                        dv = (vr - vl);
                        dw = (wr - wl);
                    } // end else
                    
                    ///////////////////////////////////////////////////////////////////////
                    
                    // test for x clipping, LHS
                    if (xstart < min_clip_x)
                    {
                        // compute x overlap
                        dx = min_clip_x - xstart;
                        
                        // slide interpolants over
                        ui+=dx*du;
                        vi+=dx*dv;
                        wi+=dx*dw;
                        
                        // set x to left clip edge
                        xstart = min_clip_x;
                        
                    } // end if
                    
                    // test for x clipping RHS
                    if (xend > max_clip_x)
                        xend = max_clip_x;
                    
                    ///////////////////////////////////////////////////////////////////////
                    
                    // draw span
                    for (xi=xstart; xi<=xend; xi++)
                    {
                        // write textel assume 5.6.5
                        screen_ptr[xi] = SDL_MapRGB(gSurface->format,
                                                (Uint8)(ui >> (FIXP16_SHIFT)),
                                                        (Uint8)(vi >> (FIXP16_SHIFT)),
                                                        (Uint8)(wi >> (FIXP16_SHIFT)));
                        
                        // interpolate u,v
                        ui+=du;
                        vi+=dv;
                        wi+=dw;
                    } // end for xi
                    
                    // interpolate u,v,w,x along right and left edge
                    xl+=dxdyl;
                    ul+=dudyl;
                    vl+=dvdyl;
                    wl+=dwdyl;
                    
                    xr+=dxdyr;
                    ur+=dudyr;
                    vr+=dvdyr;
                    wr+=dwdyr;
                    
                    // advance screen ptr
                    screen_ptr+=mem_pitch;
                    
                    // test for yi hitting second region, if so change interpolant
                    if (yi==yrestart)
                    {
                        // test interpolation side change flag
                        
                        if (irestart == INTERP_LHS)
                        {
                            // LHS
                            dyl = (y2 - y1);	
                            
                            dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
                            dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
                            dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
                            dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;  
                            
                            // set starting values
                            xl = (x1  << FIXP16_SHIFT);
                            ul = (tu1 << FIXP16_SHIFT);
                            vl = (tv1 << FIXP16_SHIFT);
                            wl = (tw1 << FIXP16_SHIFT);
                            
                            // interpolate down on LHS to even up
                            xl+=dxdyl;
                            ul+=dudyl;
                            vl+=dvdyl;
                            wl+=dwdyl;
                        } // end if
                        else
                        {
                            // RHS
                            dyr = (y1 - y2);	
                            
                            dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
                            dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
                            dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
                            dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;   		
                            
                            // set starting values
                            xr = (x2  << FIXP16_SHIFT);
                            ur = (tu2 << FIXP16_SHIFT);
                            vr = (tv2 << FIXP16_SHIFT);
                            wr = (tw2 << FIXP16_SHIFT);
                            
                            // interpolate down on RHS to even up
                            xr+=dxdyr;
                            ur+=dudyr;
                            vr+=dvdyr;
                            wr+=dwdyr;
                            
                        } // end else
                        
                    } // end if
                    
                } // end for y
                
            } // end if
            else
            {
                // no x clipping
                // point screen ptr to starting line
                screen_ptr = dest_buffer + (ystart * mem_pitch);
                
                for (yi = ystart; yi<=yend; yi++)
                {
                    // compute span endpoints
                    xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                    xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
                    
                    // compute starting points for u,v,w interpolants
                    ui = ul + FIXP16_ROUND_UP;
                    vi = vl + FIXP16_ROUND_UP;
                    wi = wl + FIXP16_ROUND_UP;
                    
                    // compute u,v interpolants
                    if ((dx = (xend - xstart))>0)
                    {
                        du = (ur - ul)/dx;
                        dv = (vr - vl)/dx;
                        dw = (wr - wl)/dx;
                    } // end if
                    else
                    {
                        du = (ur - ul);
                        dv = (vr - vl);
                        dw = (wr - wl);
                    } // end else
                    
                    // draw span
                    for (xi=xstart; xi<=xend; xi++)
                    {
                        // write textel assume 5.6.5
                        screen_ptr[xi] = SDL_MapRGB(gSurface->format,
                                                (Uint8)(ui >> (FIXP16_SHIFT)),
                                                        (Uint8)(vi >> (FIXP16_SHIFT)),
                                                        (Uint8)(wi >> (FIXP16_SHIFT)));
                        
                        // interpolate u,v
                        ui+=du;
                        vi+=dv;
                        wi+=dw;
                    } // end for xi
                    
                    // interpolate u,v,w,x along right and left edge
                    xl+=dxdyl;
                    ul+=dudyl;
                    vl+=dvdyl;
                    wl+=dwdyl;
                    
                    xr+=dxdyr;
                    ur+=dudyr;
                    vr+=dvdyr;
                    wr+=dwdyr;
                    
                    // advance screen ptr
                    screen_ptr+=mem_pitch;
                    
                    // test for yi hitting second region, if so change interpolant
                    if (yi==yrestart)
                    {
                        // test interpolation side change flag
                        
                        if (irestart == INTERP_LHS)
                        {
                            // LHS
                            dyl = (y2 - y1);	
                            if (dyl == 0) {
                                dyl = 1;
                            }
                            
                            dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
                            dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
                            dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
                            dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;   
                            
                            // set starting values
                            xl = (x1  << FIXP16_SHIFT);
                            ul = (tu1 << FIXP16_SHIFT);
                            vl = (tv1 << FIXP16_SHIFT);
                            wl = (tw1 << FIXP16_SHIFT);
                            
                            // interpolate down on LHS to even up
                            xl+=dxdyl;
                            ul+=dudyl;
                            vl+=dvdyl;
                            wl+=dwdyl;
                        } // end if
                        else
                        {
                            // RHS
                            dyr = (y1 - y2);
                            if (dyr == 0) {
                                dyr = 1;
                            }
                            
                            dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
                            dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
                            dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
                            dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;   
                            
                            // set starting values
                            xr = (x2  << FIXP16_SHIFT);
                            ur = (tu2 << FIXP16_SHIFT);
                            vr = (tv2 << FIXP16_SHIFT);
                            wr = (tw2 << FIXP16_SHIFT);
                            
                            // interpolate down on RHS to even up
                            xr+=dxdyr;
                            ur+=dudyr;
                            vr+=dvdyr;
                            wr+=dwdyr;
                        } // end else
                        
                    } // end if
                    
                } // end for y
                
            } // end else	
            
        } // end if
    
} // end Draw_Gouraud_Triangle16


/////////////////////////////////////////////////////////////////


