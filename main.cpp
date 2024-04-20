#include "matrix.h"
#include "rasterizer3D.h"
#include "model.h"

Model *model = NULL;
const int width=640;
const int height=480;
const int depth =255;
uint32_t *textureBuffer;
float *zbuffer;

Vec3f light_dir(1,1,1);
Vec3f eye(1,1,3);
Vec3f center(0,0,0);
Vec3f up (0,1,0);

struct GourandShader : public Shader {
    Vec3f varying_intensity; //written by vertex shader, read by fragment shader

    Matrix vertex(int iface, int nthvert){
        varying_intensity[nthvert] = std::max(0.f,model->v_normal(iface,nthvert)*light_dir);//get diffuse lighting intensity
        Matrix vertex=model->vert(iface,nthvert); // read the vertex from .obj file
        return Viewport*Projection*ModelView*vertex; // transform it to screen coordinates
    }

    bool fragment(Vec3f bar, uint32_t &color){
        float intensity = varying_intensity*bar; // Interpolate intensity for the current pixel
        int intensity_i=static_cast<int>(intensity*255);
        color=((intensity_i& 0xff)<<16) + ((intensity_i& 0xff)<<8)+(intensity_i& 0xff);
        return false; //we do not discard this pixel
    }
};
struct PhongShader;
int main(int argc, char * argv[]){
    if (argc==2){
        model = new Model(argv[1]);
    } else{
        model = new Model("obj/teapot.obj");
    }
    lookat(eye,center,up);
    viewport(width/8,height/8,width*3/4,height*3/4);
    projection(-1.f/(eye-center).norm());
    light_dir.normalize();

    textureBuffer= new uint32_t[ width * height ];
    zbuffer = new float[width * height];
    
    GourandShader shader;
    for(int i=0;i<model->nfaces();i++){
        Matrix screen_coords[3];
        for(int j=0;j<3;j++){
            screen_coords[j]=shader.vertex(i,j);
        }
        drawTriangle(screen_coords[0],screen_coords[1],screen_coords[2],zbuffer,((color& 0xff)<<16) + ((color& 0xff)<<8)+(color& 0xff));
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
    textureBuffer=NULL;
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    delete model;
}