#include "matrix.h"
#include "vec3f.h"
#include "types.h"
#include <cstdint>

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;

void viewport(int x,int y, int w, int h);
void projection(float coeff=0.f); // coeff = -1/c
void lookat(Vec3f eye, Vec3f center, Vec3f up);

struct Shader{
    virtual ~Shader();
    virtual Matrix vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, uint32_t &color) = 0;
};

void drawTriangle(Matrix *pts,Shader &shader,uint32_t *textureBuffer,float *zbuffer,int width, int height);