#include <iostream>
#include <vector>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

struct Point{
    int x;
    int y;
    // int z;
    // int w;
};
// bool isInsideTriangle();
std::vector<Point> zBuffer;
const int width=640;
const int height=480;
int screen[height][width]={0};
uint32_t *textureBuffer;

void drawLineLow(Point &p1, Point &p2){
    int dx=p2.x-p1.x;
    int dy=p2.y-p1.y;
    int prevy=p1.y;
    int yinc=1;
    if(dy<0){
        dy=-dy;
        yinc=-1;
    }
    int D=2*dy-dx;
    for(int x=p1.x;x<p2.x;x++){
        textureBuffer[(prevy*width) + x] = 0xFFFFFFFF;
        if(D>0){
            prevy+=yinc;
            D=D-2*dx;
        }
        D=D+2*dy;
    }
}

void drawLineHigh(Point &p1, Point &p2){
    int dx=p2.x-p1.x;
    int dy=p2.y-p1.y;
    int prevx=p1.x;
    int xinc=1;
    if(dx<0){
        dx=-dx;
        xinc=-1;
    }
    int D=2*dx-dy;
    for(int y=p1.y;y<p2.y;y++){
        textureBuffer[(y*width) + prevx] = 0xFFFFFFFF;
        if(D>0){
            prevx+=xinc;
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
// void drawCircle(Point p1, int diam){


// }
int main(int argc, char* argv[]){
    // Dimensions of screen
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("window",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
    SDL_Texture * texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height );
    textureBuffer= new uint32_t[ width * height ];
    Point p1;
    Point p2;
    p1.x=1;
    p1.y=200;
    p2.x=220;
    p2.y=100;
    drawLine(p1,p2);
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