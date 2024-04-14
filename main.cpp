#include "matrix.h"
#include "rasterizer3D.h"

Model *m = NULL;
const int width=640;
const int height=480;
const int depth =255;

Vec3f light_dir(1,1,1);
Vec3f eye(1,1,3);
Vec3f center(0,0,0);
Vec3f up (0,1,0);

struct GourandShader : public Shader {
    Vec3f varying_intensity; //written by vertex shader, read by fragment shader

    virtual Matrix vertex(int face, int vertex){
        varying_intensity[vertex] = std::max(0.f);//get diffuse lighting intensity
        Matrix vertex; // read the vertex frmo .obj file
        return Viewport*Projection*ModelView*vertex; // transform it to screen coordinates
    }

    virtual bool fragment(Vec3f bar, uint32_t color){
        float intensity = varying_intensity*bar; // Interpolate intensity for the current pixel
        color = intensity;
        return false; //we do not discard this pixel
    }
};
struct PhongShader;
int main(int argc, char * argv[]){
    m = new Model("obj/teapot.obj");
    lookat(eye,center,up);
    viewport(width/8,height/8,width*3/4,height*3/4);
    projection(-1.f/(eye-center).norm());
    light_dir.normalize();
    float scaleX=(xMax-xMin)/width;
    float scaleY=(yMax-yMin)/height;
    for(const auto& face:faces){
        Vertex p1={static_cast<int>((vertices[face.first-1].x-xMin)/scaleX),static_cast<int>((vertices[face.first-1].y-yMin)/scaleY),0};
        Vertex p2={static_cast<int>((vertices[face.second-1].x-xMin)/scaleX),static_cast<int>((vertices[face.second-1].y-yMin)/scaleY),0};
        Vertex p3={static_cast<int>((vertices[face.third-1].x-xMin)/scaleX),static_cast<int>((vertices[face.third-1].y-yMin)/scaleY),0};
        Vec3f vec1=Vec3f(vertices[face.third-1].x-vertices[face.first-1].x,vertices[face.third-1].y-vertices[face.first-1].y,vertices[face.third-1].z-vertices[face.first-1].z);
        Vec3f vec2=Vec3f(vertices[face.second-1].x-vertices[face.first-1].x,vertices[face.second-1].y-vertices[face.first-1].y,vertices[face.second-1].z-vertices[face.first-1].z);
        Vec3f normal = vec1^vec2;
        normal.normalize();
        float intensity=normal*light_dir;
        if(intensity>0){
            uint32_t color=static_cast<int>(intensity*255);
            drawTriangle(p1,p2,p3,zbuffer,((color& 0xff)<<16) + ((color& 0xff)<<8)+(color& 0xff));
        }
    }
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
    delete m;
}