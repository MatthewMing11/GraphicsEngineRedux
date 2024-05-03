#include "rasterizer3D.h"
#include "vec3f.h"
#include "matrix.h"
#include "types.h"
#include <algorithm>
#include <iostream>
#include <limits>

Matrix ModelView;
Matrix Viewport;
Matrix Projection;

Shader::~Shader(){}

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
    Viewport(2,3) = 255/2.f;
    Viewport(0,0) = w/2.f;
    Viewport(1,1) = h/2.f;
    Viewport(2,2) = 255/2.f;
}
void projection(float r){//initializes projection matrix
    Projection=Matrix::identity(4);
    Projection(3,2)=r;
}
void lookat(Vec3f eye, Vec3f center, Vec3f up){//initializes ModelView matrix
    Vec3f z=(eye-center).normalize();
    Vec3f x= (up^z).normalize();
    Vec3f y= (z^x).normalize();
    ModelView=Matrix::identity(4);
    for(int i=0;i<3;i++){
        ModelView(0,i)=x[i];
        ModelView(1,i)=y[i];
        ModelView(2,i)=z[i];
        ModelView(i,3)=-center[i];
    }
}
Vec3f barycentric(Vertex &p1,Vertex &p2,Vertex &p3, Vertex &test_p){
    float det=(p2.y-p3.y)*(p1.x-p3.x)+(p3.x-p2.x)*(p1.y-p3.y);
    //barycentric coords
    float b1=((p2.y-p3.y)*(test_p.x-p3.x)+(p3.x-p2.x)*(test_p.y-p3.y))*1.0f/det;
    float b2=((p3.y-p1.y)*(test_p.x-p3.x)+(p1.x-p3.x)*(test_p.y-p3.y))*1.0f/det;
    float b3=1-b1-b2;
    Vec3f res=Vec3f(b3,b2,b1);
    return res;
}

void drawTriangle(Matrix &p1,Matrix &p2,Matrix &p3,Shader &shader,uint32_t *textureBuffer,float *zbuffer, int width, int height){//mesh looks better in higher dimensions, might need antialiasing
    // Vertex bbox[2];
    // bbox[0].x=std::min({p1.x,p2.x,p3.x});
    // bbox[0].y=std::min({p1.y,p2.y,p3.y});
    // bbox[1].x=std::max({p1.x,p2.x,p3.x});
    // bbox[1].y=std::max({p1.y,p2.y,p3.y});
    Vec3f bboxmin(std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),0);
    Vec3f bboxmax(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),0);
    for(int j=0;j<2;j++){
        bboxmin[j]=std::min({bboxmin[j],p1(j,0)/p1(3,0),p2(j,0)/p2(3,0),p3(j,0)/p1(3,0)});
        bboxmax[j]=std::max({bboxmin[j],p1(j,0)/p1(3,0),p2(j,0)/p2(3,0),p3(j,0)/p1(3,0)});
    }
    uint32_t color=0;
    Vertex b1={p1(0,0)/p1(3,0),p1(1,0)/p1(3,0),p1(2,0)/p1(3,0)};
    Vertex b2={p2(0,0)/p2(3,0),p2(1,0)/p2(3,0),p2(2,0)/p2(3,0)};
    Vertex b3={p3(0,0)/p3(3,0),p3(1,0)/p3(3,0),p3(2,0)/p3(3,0)};
    // for(int y = bbox[0].y;y<bbox[1].y+1;y++){
    //     for(int x=bbox[0].x;x<bbox[1].x+1;x++){
    for(int y = bboxmin[1];y<bboxmax[1]+1;y++){
        for(int x=bboxmin[0];x<bboxmax[0]+1;x++){
            Vertex pixel;
            pixel.x=x;
            pixel.y=y;
            Vec3f bary_coords=barycentric(b1,b2,b3,pixel);
            if (bary_coords[0]<0 || bary_coords[1]<0 || bary_coords[2]<0) continue;
            pixel.z=b1.z*bary_coords[0]+b2.z*bary_coords[1]+b3.z*bary_coords[2];
            // pixel.z=p1.z*bary_coords[0]+p2.z*bary_coords[1]+p3.z*bary_coords[2];
            bool discard = shader.fragment(bary_coords,color);
            if(!discard && y*width +x < width*height){
                // std::cout<<y*width +x<<std::endl;
                zbuffer[y*width +x]=pixel.z;
                textureBuffer[y*width + x] = color;
            }
        }
    }
}