#include "rasterizer3D.h"
#include "vec3f.h"
#include "matrix.h"
#include "types.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <cstdlib>

Matrix ModelView;
Matrix Viewport;
Matrix Projection;

Shader::~Shader(){}

// IShader::~IShader(){}
// Line functions are deprecated unless working with 2D.
void drawLineLow(Vertex &p1, Vertex &p2,uint32_t *textureBuffer,uint32_t color,int width){
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
        textureBuffer[(curry*width) + x] = color;
        if(D>0){
            curry+=yinc;
            D=D-2*dx;
        }
        D=D+2*dy;
    }
}

void drawLineHigh(Vertex &p1, Vertex &p2,uint32_t *textureBuffer,uint32_t color,int width){
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
        textureBuffer[(y*width) + currx] = color;
        if(D>0){
            currx+=xinc;
            D=D-2*dy;
        }
        D=D+2*dx;
    }
}
void drawLine(Vertex &p1, Vertex &p2,uint32_t *textureBuffer,uint32_t color,int width){
    if (abs(p1.y-p2.y)<abs(p1.x-p2.x)){
        if (p1.x<p2.x){
            drawLineLow(p1,p2,textureBuffer,color,width);
        }else{
            drawLineLow(p2,p1,textureBuffer,color,width);
        }
    }
    else{
        if (p1.y<p2.y){
            drawLineHigh(p1,p2,textureBuffer,color,width);
        }else{
            drawLineHigh(p2,p1,textureBuffer,color,width);
        }
    }
}
void viewport(int x, int y, int w, int h){//initializes viewport matrix
    Viewport = Matrix::identity(4);
    Viewport(0,3) = x+w/2.f;
    Viewport(1,3) = y+h/2.f;
    Viewport(0,0) = w/2.f;
    Viewport(1,1) = h/2.f;
    Viewport(2,2)=0;
    Viewport(2,3)=1.f;
}
void projection(float r){//initializes projection matrix
    Projection=Matrix::identity(4);
    Projection(3,2)=r;
}
void lookat(Vec3f eye, Vec3f center, Vec3f up){//initializes ModelView matrix
    Vec3f z=(eye-center).normalize();
    Vec3f x= (up^z).normalize();
    Vec3f y= (z^x).normalize();
    // ModelView=Matrix::identity(4);
    Matrix Minv=Matrix::identity();
    Matrix Tr = Matrix::identity();
    for(int i=0;i<3;i++){
        // ModelView(0,i)=x[i];
        // ModelView(1,i)=y[i];
        // ModelView(2,i)=z[i];
        // ModelView(i,3)=-center[i];
        Minv(0,i)=x[i];
        Minv(1,i)=y[i];
        Minv(2,i)=z[i];
        Tr(i,3)=-center[i];
    }
    ModelView=Minv*Tr;
}
Vec3f barycentric(Vertex &p1,Vertex &p2,Vertex &p3, Vertex &test_p){
    Vec3f s[2];
    Vec3f A={p1.x,p1.y,p1.z};
    Vec3f B={p2.x,p2.y,p2.z};
    Vec3f C={p3.x,p3.y,p3.z};
    Vec3f P= {test_p.x,test_p.y,test_p.z};
    for(int i=2;i--;){
        s[i][0]=C[i]-A[i];
        s[i][1]=B[i]-A[i];
        s[i][2]=A[i]-P[i];
    }
    Vec3f u = s[0]^s[1];
    if(std::abs(u[2])>1e-2)
        return Vec3f(1.f-(u[0]+u[1])/u[2],u[1]/u[2],u[0]/u[2]);
    return Vec3f(-1,1,1);
}

// void drawTriangle(Matrix *pts,Shader &shader,uint32_t *textureBuffer,float *zbuffer, int width, int height){//mesh looks better in higher dimensions, might need antialiasing
//     // Vertex bbox[2];
//     // bbox[0].x=std::min({p1.x,p2.x,p3.x});
//     // bbox[0].y=std::min({p1.y,p2.y,p3.y});
//     // bbox[1].x=std::max({p1.x,p2.x,p3.x});
//     // bbox[1].y=std::max({p1.y,p2.y,p3.y});
//     Vec3f bboxmin(std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),0);
//     Vec3f bboxmax(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),0);
//     for(int i=0;i<3;i++){
//         for(int j=0;j<2;j++){
//             bboxmin[j]=std::min({bboxmin[j],pts[i](j,0)/pts[i](3,0)});
//             bboxmax[j]=std::max({bboxmax[j],pts[i](j,0)/pts[i](3,0)});
//         }
//     }
//     uint32_t color=0;
//     Vertex b1={pts[0](0,0)/pts[0](3,0),pts[0](1,0)/pts[0](3,0),pts[0](2,0)/pts[0](3,0)};
//     Vertex b2={pts[1](0,0)/pts[1](3,0),pts[1](1,0)/pts[1](3,0),pts[1](2,0)/pts[1](3,0)};
//     Vertex b3={pts[2](0,0)/pts[2](3,0),pts[2](1,0)/pts[2](3,0),pts[2](2,0)/pts[2](3,0)};
//     // for(int y = bbox[0].y;y<bbox[1].y+1;y++){
//     //     for(int x=bbox[0].x;x<bbox[1].x+1;x++){
//     for(int y = bboxmin[1];y<bboxmax[1]+1;y++){
//         for(int x=bboxmin[0];x<bboxmax[0]+1;x++){
//             Vertex pixel;
//             pixel.x=x;
//             pixel.y=y;
//             Vec3f bary_coords=barycentric(b1,b2,b3,pixel);
//             pixel.z = pts[0](2,0)*bary_coords[0]+pts[1](2,0)*bary_coords[1]+pts[2](2,0)*bary_coords[2];
//             float w=pts[0](3,0)*bary_coords[0]+pts[1](3,0)*bary_coords[1]+pts[2](3,0)*bary_coords[2];
//             int frag_depth=std::max(0,std::min(255,int(pixel.z/w+.5)));
//             if (bary_coords[0]<0 || bary_coords[1]<0 || bary_coords[2]<0 ||zbuffer[y*width+x]>frag_depth) continue;
//             // pixel.z=b1.z*bary_coords[0]+b2.z*bary_coords[1]+b3.z*bary_coords[2];
//             // pixel.z=p1.z*bary_coords[0]+p2.z*bary_coords[1]+p3.z*bary_coords[2];
//             bool discard = shader.fragment(bary_coords,color);
//             if(!discard ){//&& y*width +x < width*height
//                 // std::cout<<y*width +x<<std::endl;
//                 zbuffer[y*width +x]=frag_depth;
//                 textureBuffer[y*width + x] = color;
//             }
//         }
//     }
// }
void drawTriangle(Matrix &clipc, Shader &shader, uint32_t *textureBuffer,float *zbuffer, int width, int height) {//specifically for Phong shading and Ambient Occlusion
    Matrix pts  = (Viewport*clipc).transpose(); // transposed to ease access to each of the points
    Matrix pts2 = Matrix(3,2);
    for (int i=0; i<3; i++) {
        for(int j=0;j<2;j++){
            pts2(i,j) =(pts(i,j)/pts(i,3));
        }
    }
    Vec3f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max(),0);
    Vec3f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(),0);
    Vec3f clamp(width-1, height-1,0);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts2(i,j)));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2(i,j)));
        }
    }
    uint32_t color=0;
    Vertex b1={pts2(0,0),pts2(0,1),0};
    Vertex b2={pts2(1,0),pts2(1,1),0};
    Vertex b3={pts2(2,0),pts2(2,1),0};
    for(int y = bboxmin[1];y<bboxmax[1]+1;y++){
        for(int x=bboxmin[0];x<bboxmax[0]+1;x++){
            Vertex pixel;
            pixel.x=x;
            pixel.y=y;
            Vec3f bc_screen  = barycentric(b1,b2,b3,pixel);
            Vec3f bc_clip    = Vec3f(bc_screen[0]/pts(0,3), bc_screen[1]/pts(1,3), bc_screen[2]/pts(2,3));
            bc_clip=bc_clip/(bc_clip[0]+bc_clip[1]+bc_clip[2]);
            float frag_depth = Vec3f(clipc(2,0),clipc(2,1),clipc(2,2))*bc_clip;
            if (bc_screen[0]<0 || bc_screen[1]<0 || bc_screen[2]<0 || zbuffer[y*width+x]>frag_depth) continue;
            // bool discard = shader.fragment(bc_clip, color);
            bool discard =shader.fragment(Vec3f(pixel.x,pixel.y,frag_depth),bc_clip,color);
            if (!discard) {
                zbuffer[y*width +x]=frag_depth;
                textureBuffer[y*width + x] = color;
            }
        }
    }
}