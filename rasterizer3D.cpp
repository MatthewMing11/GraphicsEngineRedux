#include "rasterizer3D.cpp"
void drawLineLow(Vertex &p1, Vertex &p2,uint32_t *textureBuffer,uint32_t color){
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

void drawLineHigh(Vertex &p1, Vertex &p2,uint32_t *textureBuffer,uint32_t color){
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
void drawLine(Vertex &p1, Vertex &p2,uint32_t *textureBuffer,uint32_t color){
    if (abs(p1.y-p2.y)<abs(p1.x-p2.x)){
        if (p1.x<p2.x){
            drawLineLow(p1,p2,color);
        }else{
            drawLineLow(p2,p1,color);
        }
    }
    else{
        if (p1.y<p2.y){
            drawLineHigh(p1,p2,color);
        }else{
            drawLineHigh(p2,p1,color);
        }
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

void drawTriangle(Vertex &p1,Vertex &p2,Vertex &p3,Shader shader,uint32_t *textureBuffer,float *zbuffer){//mesh looks better in higher dimensions, might need antialiasing
    // wireframe of triangle
    // drawLine(p1,p2,color);
    // drawLine(p2,p3,color);
    // drawLine(p3,p1,color);
    Vertex bbox[2];
    bbox[0].x=std::min({p1.x,p2.x,p3.x});
    bbox[0].y=std::min({p1.y,p2.y,p3.y});
    bbox[1].x=std::max({p1.x,p2.x,p3.x});
    bbox[1].y=std::max({p1.y,p2.y,p3.y});
    for(int y = bbox[0].y;y<bbox[1].y+1;y++){
        for(int x=bbox[0].x;x<bbox[1].x+1;x++){
            Vertex pixel;
            pixel.x=x;
            pixel.y=y;
            Vec3f bary_coords=barycentric(p1,p2,p3,pixel);
            if (bary_coords[0]<0 || bary_coords[1]<0 || bary_coords[2]<0) continue;
            pixel.z=p1.z*bary_coords[0]+p2.z*bary_coords[1]+p3.z*bary_coords[2];
            if(zbuffer[y*width+x]<pixel.z){
                zbuffer[y*width +x]=pixel.z;
                textureBuffer[y*width + x] = color;
            }
        }
    }
}