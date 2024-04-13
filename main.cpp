#include "matrix.h"
#include "rasterizer3D.h"

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
    Model m = new Model("obj/teapot.obj");
    // delete m;
}