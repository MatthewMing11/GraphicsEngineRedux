#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <limits>
#include <cmath>
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

struct Vertex{
    float x;
    float y;
    float z;
};

struct Face{
    int first;
    int second;
    int third;
};
class Vec3f{//inspired by Vec3f java graphics class
    private:
        float arr[3];
    public:
        Vec3f(float x,float y, float z)
        {
            arr[0]=x;
            arr[1]=y;
            arr[2]=z;
        }
        Vec3f operator+(Vec3f const& obj)
        {
            Vec3f res=Vec3f(arr[0]+obj.arr[0],arr[1]+obj.arr[1],arr[2]+obj.arr[2]);
            return res;
        }
        Vec3f operator-(Vec3f const& obj)
        {
            Vec3f res=Vec3f(arr[0]-obj.arr[0],arr[1]-obj.arr[1],arr[2]-obj.arr[2]);
            return res;
        }
        float operator*(Vec3f const& obj)//dot product
        {
            return arr[0]*obj.arr[0]+arr[1]*obj.arr[1]+arr[2]*obj.arr[2];
        }
        Vec3f operator^(Vec3f const& obj)//cross product
        {
            Vec3f res=Vec3f(arr[1]*obj.arr[2]-arr[2]*obj.arr[1],arr[2]*obj.arr[0]-arr[0]*obj.arr[2],arr[0]*obj.arr[1]-arr[1]*obj.arr[0]);
            return res;
        }
        float operator[](int index){
            return arr[index];
        }
        Vec3f normalize()//modifies itself and returns for convenience
        {
            if(arr[0]==0 && arr[1]==0 && arr[2]==0)//degenerate case zero vector
            {
                return *this;
            }
            float mag = std::sqrt(std::pow(arr[0],2)+std::pow(arr[1],2)+std::pow(arr[2],2));
            arr[0]=arr[0]/mag;
            arr[1]=arr[1]/mag;
            arr[2]=arr[2]/mag;
            Vec3f res=Vec3f(arr[0],arr[1],arr[2]);
            return res;
        }
};
class Matrix{
    private:
        float **arr;
        int w;
        int h;
    public:
        Matrix()=default;
        Matrix(int row,int col){
            arr = new float*[row];
            for(int i=0;i<row;++i){
                arr[i]= new float[col];
            }
            for(int i=0;i<row;++i){
                for(int j=0;j<col;++j){
                    arr[i][j]=0;
                }
            }
            w=col;
            h=row;
        }
        Matrix(Vec3f vec){
            Matrix m=Matrix(1,3);
            for(int i=0;i<3;i++){
                m[0][i]=vec[i];
            }
        }
        static Matrix identity(int size=4){
            Matrix m = Matrix(size,size);
            for(int i=0;i<size;++i){
                m[i][i]=1;
            }
            return m;
        } 
        float* operator[](int index){
            return arr[index];
        }
        Matrix operator*(Matrix const& obj)//matrix multiplication
        {
            Matrix res=Matrix(h,obj.w);
            for (int i=0;i<h;i++){
                for(int j=0;j<obj.w;j++){
                    for(int k=0;k<obj.h;k++){
                        res[i][j]+=arr[i][k]*obj.arr[k][j];
                    }
                }
            }
            return res;
        }
        ~Matrix(){
            for(int i=0;i<h;i++){
                delete[] arr[i];
            }
            delete[] arr;
        }
};
const int width=640;
const int height=480;
const int depth =255;
Matrix ModelView;
Matrix Viewport;
Matrix Projection;
int screen[height][width]={0};
uint32_t *textureBuffer;

void drawLineLow(Vertex &p1, Vertex &p2,uint32_t color){
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

void drawLineHigh(Vertex &p1, Vertex &p2,uint32_t color){
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
void drawLine(Vertex &p1, Vertex &p2,uint32_t color){
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
void drawCircle(Vertex &p1, int radius){
    int fast_coord=0;//the addend for the coordinate that always increments
    int slow_coord=radius;//the addend for the coordinate that sometimes increments
    int E=3-2*radius;// E is the change in the decision formula used to determine whether or not to decrement the slow coord
    // if (radius>0){
    //     drawCircle(p1,radius-1);
    // }
    int x=static_cast<int>(p1.x);
    int y=static_cast<int>(p1.y);
    while(true){
        textureBuffer[((y+fast_coord)*width) + (x+slow_coord)] = 0xFFFFFFFF;// Octant 1
        textureBuffer[((y+slow_coord)*width) + (x+fast_coord)] = 0xFFFFFFFF;// Octant 2
        textureBuffer[((y+slow_coord)*width) + (x-fast_coord)] = 0xFFFFFFFF;// Octant 3
        textureBuffer[((y+fast_coord)*width) + (x-slow_coord)] = 0xFFFFFFFF;// Octant 4
        textureBuffer[((y-fast_coord)*width) + (x-slow_coord)] = 0xFFFFFFFF;// Octant 5
        textureBuffer[((y-slow_coord)*width) + (x-fast_coord)] = 0xFFFFFFFF;// Octant 6
        textureBuffer[((y-slow_coord)*width) + (x+fast_coord)] = 0xFFFFFFFF;// Octant 7
        textureBuffer[((y-fast_coord)*width) + (x+slow_coord)] = 0xFFFFFFFF;// Octant 8
        // Generates pixels based on octant 1
        if(E>0){
            E=E+2*(5-2*slow_coord+2*fast_coord);
            slow_coord-=1;
        }else{
            E=E+2*(3+2*fast_coord);
        }
        fast_coord+=1;
        if(fast_coord>slow_coord){
            break;
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

void drawTriangle(Vertex &p1,Vertex &p2,Vertex &p3,float *zbuffer,uint32_t color){//mesh looks better in higher dimensions, might need antialiasing
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
void lookat(Vec3f eye, Vec3f center, Vec3f up){//initializes ModelView matrix
    Vec3f z=(eye-center).normalize();
    Vec3f x= (up^z).normalize();
    Vec3f y= (z^x).normalize();
    Matrix Minv=Matrix::identity();
    Matrix Tr= Matrix::identity();
    for(int i=0;i<3;i++){
        Minv[0][i]=x[i];
        Minv[1][i]=y[i];
        Minv[2][i]=z[i];
        Tr[i][3]=-eye[i];
    }
    ModelView = Minv*Tr;
}
void viewport(int x, int y, int w, int h){//initializes viewport matrix
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;
    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    Viewport= m;
}
void projection(float r){//initializes projection matrix
    Matrix m=Matrix::identity(4);
    m[3][2]=r;
    Projection= m;
}
int main(int argc, char* argv[]){
    // Dimensions of screen
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("window",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
    SDL_Texture * texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height );
    textureBuffer= new uint32_t[ width * height ];
    float *zbuffer = new float[width * height];
    for(int i=0;i<width*height;i++){
        zbuffer[i]=std::numeric_limits<int>::min();
    }
    std::ifstream file("obj/teapot.obj");
    std::string line;
    std::vector<std::vector<std::string>> info;//for debugging
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    Vec3f light_dir=Vec3f(0,0,-1);
    float xMin=0,xMax=0,
        yMin=0,yMax=0,
        zMin=0,zMax=0;
    while(std::getline(file,line))
    {
        if(line.size()==0){//ignore blank lines
            continue;
        }
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        while(std::getline(iss,token,' '))//tokenizing line
        {
            tokens.push_back(token);
        }
        info.push_back(tokens);
        if (tokens[0]=="v"){
            Vertex data = {std::stof(tokens[1]),std::stof(tokens[2]),std::stof(tokens[3])};
            xMin=std::min(xMin,data.x);
            yMin=std::min(yMin,data.y);
            zMin=std::min(zMin,data.z);
            xMax=std::max(xMax,data.x);
            yMax=std::max(yMax,data.y);
            zMax=std::max(zMax,data.z);
            vertices.push_back(data);
        }
        if (tokens[0]=="f"){
            Face data={std::stoi(tokens[1]),std::stoi(tokens[2]),std::stoi(tokens[3])};
            faces.push_back(data);
        }
    }
    // for(const auto& v:info){
    //     for(const auto& t:v){
    //         std::cout<<t<<" ";
    //     }
    //     std::cout<<std::endl;
    // }
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
}