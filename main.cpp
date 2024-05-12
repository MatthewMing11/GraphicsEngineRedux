#include "matrix.h"
#include "vec3f.h"
#include "rasterizer3D.h"
#include "model.h"
#include <iostream>
#include <limits>

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
        Matrix vertex=Matrix(model->vert(iface,nthvert),1); // read the vertex from .obj file
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
struct PhongReflectShader : public Shader {
    Matrix varying_uv=Matrix(2,3);  // same as above
    Matrix uniform_M=Matrix(4,4);   //  Projection*ModelView
    Matrix uniform_MIT= Matrix(4,4); // (Projection*ModelView).invert_transpose()

    virtual Matrix vertex(int iface, int nthvert) {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Matrix vertex =  Matrix(model->vert(iface, nthvert),1); // read the vertex from .obj file
        return Viewport*Projection*ModelView*vertex; // transform it to screen coordinates
    }

    virtual bool fragment(Vec3f bar, uint32_t &color) {
        Vec3f uv = varying_uv*bar;
        Matrix normal=uniform_MIT*Matrix(model->normal(uv),1);
        Vec3f n = Vec3f(normal(0,0),normal(1,0),normal(2,0)).normalize();
        Matrix light=uniform_M  *Matrix(light_dir,1);
        Vec3f l = Vec3f(light(0,0),light(1,0),light(2,0)).normalize();
        Vec3f r = (n*(n*l*2.f) - l).normalize();   // reflected light
        float spec = pow(std::max(r[2], 0.0f), model->specular(uv));
        float diff = std::max(0.f, n*l);
        uint32_t c = model->diffuse(uv);
        // std::cout<<"color:"<<c<<std::endl;
        color=(static_cast<int>(std::min<float>(5 + (c>>16)*(diff + .6*spec), 255))<<16) + (static_cast<int>(std::min<float>(5 + ((c>>8)&0xff)*(diff + .6*spec), 255))<<8)+static_cast<int>(std::min<float>(5 + (c&0xff)*(diff + .6*spec), 255));
        return false;
    }
}; 
struct PhongShader : public Shader {
    Matrix varying_uv=Matrix(2,3);  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    Matrix varying_nrm=Matrix(3,3); // normal per vertex to be interpolated by FS
    Matrix varying_tri=Matrix(4,3); // to be used externally for phong shading
    Matrix ndc_tri=Matrix(3,3);     // triangle in normalized device coordinates

    virtual Matrix vertex(int iface, int nthvert) {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Matrix normal =(Projection*ModelView).invert_transpose()*Matrix(model->v_normal(iface, nthvert), 0.f);
        varying_nrm.set_col(nthvert, Vec3f(normal(0,0),normal(1,0),normal(2,0)));
        Matrix vertex = Projection*ModelView*Matrix(model->vert(iface, nthvert),1);
        // for(int i=0;i<4;i++){
        //     std::cout<<vertex(i,0)<<" ";
        // }
        // std::cout<<std::endl;
        varying_tri.set_col(nthvert, Vec3f(vertex(0,0),vertex(1,0),vertex(2,0)));
        // std::cout<<vertex(3,0)<<" missing"<<std::endl;
        varying_tri(3,nthvert)=vertex(3,0);//temp fix for w coord not being read in set_col
        ndc_tri.set_col(nthvert, Vec3f(vertex(0,0)/vertex(3,0),vertex(1,0)/vertex(3,0),vertex(2,0)/vertex(3,0)));
        return vertex;
    }

    virtual bool fragment(Vec3f bar, uint32_t &color) {
        // std::cout<<"varying_nrm"<<std::endl;
        // for(int i=0;i<3;i++){
        //     for(int j=0;j<3;j++){
        //         std::cout<<varying_nrm(i,j)<<" ";
        //     }
        //     std::cout<<std::endl;
        // }
        // std::cout<<"varying_uv"<<std::endl;
        // for(int i=0;i<2;i++){
        //     for(int j=0;j<3;j++){
        //         std::cout<<varying_nrm(i,j)<<" ";
        //     }
        //     std::cout<<std::endl;
        // }
        Vec3f bn = (varying_nrm*bar).normalize();
        Vec3f uv = varying_uv*bar;
        // float diff = std::max(0.f, bn*light_dir);
        Matrix A=Matrix(3,3);
        for(int i=0;i<3;i++){
            A(0,i) = (ndc_tri.col(1) - ndc_tri.col(0))[i];
            A(1,i) = (ndc_tri.col(2) - ndc_tri.col(0))[i];
            A(2,i) = bn[i];
        }
        Matrix AI= A.invert();
        Vec3f first=Vec3f(varying_uv(0,1) - varying_uv(0,0), varying_uv(0,2) - varying_uv(0,0), 0);
        Vec3f second = Vec3f(varying_uv(1,1) - varying_uv(1,0), varying_uv(1,2) - varying_uv(1,0), 0);
        Vec3f i = AI * first;
        Vec3f j = AI * second;

        Matrix B=Matrix(3,3);
        B.set_col(0, i.normalize());
        B.set_col(1, j.normalize());
        B.set_col(2, bn);
        Vec3f normal = model->normal(uv);
        Vec3f n = (B*normal).normalize();

        float diff = std::max(0.f, n*light_dir);
        color = model->diffuse(uv)*diff;
        // std::cout<<color<<std::endl;
        return false;
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
    // Matrix light=(Projection*ModelView*Matrix(light_dir, 0.f));
    // light_dir=Vec3f(light(0,0),light(1,0),light(2,0));
    light_dir.normalize();

    textureBuffer= new uint32_t[ width * height ];
    zbuffer = new float[width * height];
    // for (int i=width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());
    
    // GourandShader shader;
    // for(int i=0;i<model->nfaces();i++){
    //     // Vertex screen_coords[3];
    //     Matrix screen_coords[3];
    //     for(int j=0;j<3;j++){
    //         // Vertex v={shader.vertex(i,j)(0,0),shader.vertex(i,j)(1,0),shader.vertex(i,j)(2,0)};
    //         // screen_coords[j]=v;
    //         screen_coords[j]=shader.vertex(i,j);
    //     }
    //     drawTriangle(screen_coords,shader,textureBuffer,zbuffer,width,height);
    // }
    // PhongReflectShader shader;
    // shader.uniform_M   =  Projection*ModelView;
    // shader.uniform_MIT = (Projection*ModelView).invert_transpose();
    // for (int i=0; i<model->nfaces(); i++) {
    //     Matrix screen_coords[3];
    //     for (int j=0; j<3; j++) {
    //         screen_coords[j] = shader.vertex(i, j);
    //     }
    //     drawTriangle(screen_coords,shader,textureBuffer,zbuffer,width,height);
    // }
    PhongShader shader;
    for (int i=0; i<model->nfaces(); i++) {
        for (int j=0; j<3; j++) {
            shader.vertex(i, j);
        }
        // std::cout<<i<<std::endl;
        drawTriangle(shader.varying_tri, shader,textureBuffer,zbuffer,width,height);
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