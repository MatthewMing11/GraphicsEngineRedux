#include "matrix.h"
#include "vec3f.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;

void viewport(int x,int y, int w, int h);
void projection(float coeff=0.f); // coeff = -1/c
void lookat(Vec3f eye, Vec3f center, Vec3f up);

struct Shader{
    virtual ~Shader();
    virtual Vec3f vertex(int face, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, uint_32 color)=0;
}

void drawTriangle(Vertex &p1,Vertex &p2,Vertex &p3,Shader &shader,uint32_t *textureBuffer,float *zbuffer);