#include <iostream>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

struct Point{
    int x;
    int y;
};
const int width=640;
const int height=480;
int screen[height][width]={0};
uint32_t *textureBuffer;

void drawLineLow(Point &p1, Point &p2){
    int dx=p2.x-p1.x;
    int dy=p2.y-p1.y;
    int curry=p1.y;
    int yinc=1;
    if(dy<0){
        dy=-dy;
        yinc=-1;
    }
    int D=2*dy-dx;
    for(int x=p1.x;x<p2.x;x++){
        textureBuffer[(curry*width) + x] = 0xFFFFFFFF;
        if(D>0){
            curry+=yinc;
            D=D-2*dx;
        }
        D=D+2*dy;
    }
}

void drawLineHigh(Point &p1, Point &p2){
    int dx=p2.x-p1.x;
    int dy=p2.y-p1.y;
    int currx=p1.x;
    int xinc=1;
    if(dx<0){
        dx=-dx;
        xinc=-1;
    }
    int D=2*dx-dy;
    for(int y=p1.y;y<p2.y;y++){
        textureBuffer[(y*width) + currx] = 0xFFFFFFFF;
        if(D>0){
            currx+=xinc;
            D=D-2*dy;
        }
        D=D+2*dx;
    }
}
void drawLine(Point &p1, Point &p2){
    if (abs(p1.y-p2.y)<abs(p1.x-p2.x)){
        if (p1.x<p2.x){
            drawLineLow(p1,p2);
        }else{
            drawLineLow(p2,p1);
        }
    }
    else{
        if (p1.y<p2.y){
            drawLineHigh(p1,p2);
        }else{
            drawLineHigh(p2,p1);
        }
    }
}
void drawCircle(Point &p1, int radius){
    int fast_coord=0;//the addend for the coordinate that always increments
    int slow_coord=radius;//the addend for the coordinate that sometimes increments
    int E=3-2*radius;// E is the change in the decision formula used to determine whether or not to decrement the slow coord
    // if (radius>0){
    //     drawCircle(p1,radius-1);
    // }
    while(true){
        textureBuffer[((p1.y+fast_coord)*width) + (p1.x+slow_coord)] = 0xFFFFFFFF;// Octant 1
        textureBuffer[((p1.y+slow_coord)*width) + (p1.x+fast_coord)] = 0xFFFFFFFF;// Octant 2
        textureBuffer[((p1.y+slow_coord)*width) + (p1.x-fast_coord)] = 0xFFFFFFFF;// Octant 3
        textureBuffer[((p1.y+fast_coord)*width) + (p1.x-slow_coord)] = 0xFFFFFFFF;// Octant 4
        textureBuffer[((p1.y-fast_coord)*width) + (p1.x-slow_coord)] = 0xFFFFFFFF;// Octant 5
        textureBuffer[((p1.y-slow_coord)*width) + (p1.x-fast_coord)] = 0xFFFFFFFF;// Octant 6
        textureBuffer[((p1.y-slow_coord)*width) + (p1.x+fast_coord)] = 0xFFFFFFFF;// Octant 7
        textureBuffer[((p1.y-fast_coord)*width) + (p1.x+slow_coord)] = 0xFFFFFFFF;// Octant 8
        // Generates pixels based on octant 1
        if(E>0){
            E=E+2*(5-2*slow_coord+2*fast_coord);
            slow_coord-=1;
        }else{
            E=E+2*(3+2*fast_coord);
        }
        fast_coord+=1;
        if(fast_coord>slow_coord){
            break;
        }
    }
}
bool isInsideTriangle(Point &p1,Point &p2,Point &p3, Point &test_p){
    float det=(p2.y-p3.y)*(p1.x-p3.x)+(p3.x-p2.x)*(p1.y-p3.y);
    //barycentric coords
    float b1=((p2.y-p3.y)*(test_p.x-p3.x)+(p3.x-p2.x)*(test_p.y-p3.y))*1.0f/det;
    float b2=((p3.y-p1.y)*(test_p.x-p3.x)+(p1.x-p3.x)*(test_p.y-p3.y))*1.0f/det;
    float b3=1-b1-b2;
    return  b1>0 && b2>0 && b3>0;
}

void drawTriangle(Point &p1,Point &p2,Point &p3){
    drawLine(p1,p2);
    drawLine(p2,p3);
    drawLine(p3,p1);
    Point bbox[2];
    bbox[0].x=std::min({p1.x,p2.x,p3.x});
    bbox[0].y=std::min({p1.y,p2.y,p3.y});
    bbox[1].x=std::max({p1.x,p2.x,p3.x});
    bbox[1].y=std::max({p1.y,p2.y,p3.y});
    for(int y = bbox[0].y;y<bbox[1].y;y++){
        for(int x=bbox[0].x;x<bbox[1].x;x++){
            Point pixel;
            pixel.x=x;
            pixel.y=y;
            if(isInsideTriangle(p1,p2,p3,pixel)){
                textureBuffer[y*width + x] = 0xFFFFFFFF;
            }
        }
    }

}
int main(int argc, char* argv[]){
    // Dimensions of screen
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("window",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
    SDL_Texture * texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height );
    textureBuffer= new uint32_t[ width * height ];
    Point p1={1,200};
    Point p2={220,100};
    drawLine(p1,p2);
    Point p3={250,250};
    drawCircle(p3,100);
    Point p4={300,300};
    Point p5={300,450};
    Point p6={420,420};
    drawTriangle(p4,p5,p6);
    SDL_UpdateTexture( texture , NULL, textureBuffer, width * sizeof (uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,texture, NULL,NULL);
    SDL_RenderPresent(renderer);
    while (true) {
    SDL_Event event;
    SDL_PollEvent(&event) ;
    if (event.type == SDL_QUIT) {
        break;
    }
    }
    SDL_Quit();
    delete textureBuffer;
    textureBuffer=NULL;
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
}