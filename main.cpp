#include "matrix.h"
#include "vec3f.h"
#include "rasterizer3D.h"
#include "model.h"
#include <iostream>

Model *model = NULL;
const int width=640;
const int height=480;
uint32_t *textureBuffer;
float *zbuffer;

Vec3f light_dir(1,1,1);
Vec3f eye(1,1,3);
Vec3f center(0,0,0);
Vec3f up (0,1,0);

struct GourandShader : public Shader {
    Vec3f varying_intensity; //written by vertex shader, read by fragment shader

    virtual Matrix vertex(int iface, int nthvert){
        varying_intensity[nthvert] = std::max(0.f,model->v_normal(iface,nthvert)*light_dir);//get diffuse lighting intensity
        Matrix vertex=model->vert(iface,nthvert); // read the vertex from .obj file
        // std::cout<<"V and P"<<std::endl;//stalls when separated, will fix this behavior later
        // Viewport=Viewport * Projection;
        // std::cout<<"V and M"<<std::endl;
        // Viewport=Viewport * ModelView;
        // std::cout<<"V and v"<<std::endl;
        // Viewport= Viewport * vertex;
        // return Viewport;
        return Viewport*Projection*ModelView*vertex; // transform it to screen coordinates
    }

    virtual bool fragment(Vec3f bar, uint32_t &color){
        float intensity = varying_intensity*bar; // Interpolate intensity for the current pixel
        int intensity_i=static_cast<int>(intensity*255);
        color=((intensity_i& 0xff)<<16) + ((intensity_i& 0xff)<<8)+(intensity_i& 0xff);
        return false; //we do not discard this pixel
    }
    // virtual bool fragment(Vec3f bar, uint32_t &color){
    //     float intensity = varying_intensity*bar; // Interpolate intensity for the current pixel
    //     if (intensity >.85) intensity =1;
    //     else if (intensity > .60) intensity =.80;
    //     else if (intensity > .45) intensity = .60;
    //     else if (intensity > .30) intensity = .45;
    //     else if (intensity > .15) intensity = .30;
    //     else intensity =0;
    //     color=((static_cast<int>(intensity*255)& 0xff)<<16) + ((static_cast<int>(intensity*155)& 0xff)<<8);
    //     return false; //we do not discard this pixel
    // }
};
struct TextureShader : public Shader {
    Vec3f          varying_intensity; // written by vertex shader, read by fragment shader
    Matrix varying_uv= Matrix(2,3);        // same as above

    virtual Matrix vertex(int iface, int nthvert) {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert)*light_dir); // get diffuse lighting intensity
        Matrix vertex = model->vert(iface, nthvert); // read the vertex from .obj file
        return Viewport*Projection*ModelView*vertex; // transform it to screen coordinates
    }
    
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        float intensity = varying_intensity*bar;   // interpolate intensity for the current pixel
        Vec3f uv = varying_uv*bar;                 // interpolate uv for the current pixel
        color = model->diffuse(uv)*intensity;      // well duh
        return false;                              // no, we do not discard this pixel
    }
}; 
struct PhongShader : public Shader {
    Vec3f          varying_intensity; // written by vertex shader, read by fragment shader
    Matrix varying_uv;        // same as above
    Matrix uniform_M;
    Matrix uniform_MIT;

    virtual Matrix vertex(int iface, int nthvert) {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert)*light_dir); // get diffuse lighting intensity
        Matrix vertex = model->vert(iface, nthvert); // read the vertex from .obj file
        return Viewport*Projection*ModelView*vertex; // transform it to screen coordinates
    }
    
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        float intensity = varying_intensity*bar;   // interpolate intensity for the current pixel
        Vec3f uv = varying_uv*bar;                 // interpolate uv for the current pixel
        color = model->diffuse(uv)*intensity;      // well duh
        return false;                              // no, we do not discard this pixel
    }
}; 
int main(int argc, char * argv[]){
    if (argc==2){
        model = new Model(argv[1],width,height);
    } else{
        model = new Model("obj/african_head.obj",width,height);
    }
    lookat(eye,center,up);
    viewport(width/8,height/8,width*3/4,height*3/4);
    projection(-1.f/(eye-center).norm());
    light_dir.normalize();

    textureBuffer= new uint32_t[ width * height ];
    zbuffer = new float[width * height];
    
    GourandShader shader;
    for(int i=0;i<model->nfaces();i++){
        // Vertex screen_coords[3];
        Matrix screen_coords[3];
        for(int j=0;j<3;j++){
            // Vertex v={shader.vertex(i,j)(0,0),shader.vertex(i,j)(1,0),shader.vertex(i,j)(2,0)};
            // screen_coords[j]=v;
            screen_coords[j]=shader.vertex(i,j);
        }
        drawTriangle(screen_coords,shader,textureBuffer,zbuffer,width,height);
    }
    // sdl code to render object in window
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("window",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
    SDL_Texture * texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height );
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
    textureBuffer=nullptr;
    delete zbuffer;
    zbuffer=nullptr;
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    delete model;
}