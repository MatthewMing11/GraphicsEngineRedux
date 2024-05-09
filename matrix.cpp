#include "matrix.h"
#include "vec3f.h"
#include <iostream>
#include <stdexcept>

Matrix::Matrix(){
    arr=nullptr;
    w=0;
    h=0;
}
Matrix::Matrix(int row,int col){
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
Matrix::Matrix(Vec3f vec,float val){
    arr=new float*[4];
    w=1;
    h=4;
    for(int i=0;i<4;++i){
        arr[i]= new float[1];
    }
    for (int i=0;i<3;++i){
        arr[i][0]=vec[i];
    }
    arr[3][0]=val;
}
Matrix::Matrix(Vertex v,float val){
    arr=new float*[4];
    w=1;
    h=4;
    for(int i=0;i<4;++i){
        arr[i]= new float[1];
    }
    arr[0][0]=v.x;
    arr[1][0]=v.y;
    arr[2][0]=v.z;
    arr[3][0]=val;
}
Matrix::Matrix(const Matrix& other): w(other.w),h(other.h){
    arr = new float*[h];
    for(int i=0;i<h;++i){
        arr[i]= new float[w];
    }
    for(int i=0;i<h;++i){
        for(int j=0;j<w;++j){
            arr[i][j]= other.arr[i][j];
        }
    }
}
Matrix& Matrix::operator=(const Matrix& other){
    for(int i=0;i<h;i++){
        delete[] arr[i];
    }
    delete[] arr;
    h=other.h;
    w=other.w;
    arr = new float*[h];
    for(int i=0;i<h;++i){
        arr[i]= new float[w];
    }
    for(int i=0;i<h;++i){
        for(int j=0;j<w;++j){
            arr[i][j]= other.arr[i][j];
        }
    }
    return *this;
}
Matrix Matrix::identity(int size){
    Matrix m = Matrix(size,size);
    for(int i=0;i<size;++i){
        m(i,i)=1;
    }
    return m;
} 
float& Matrix::operator()(int x, int y){
    return arr[x][y];
}

const float& Matrix::operator()(int x, int y) const {
    return arr[x][y];
} 
Matrix Matrix::operator*(Matrix const& obj)//matrix multiplication
{
    Matrix res=Matrix(h,obj.w);
    for (int i=0;i<h;i++){
        for(int j=0;j<obj.w;j++){
            for(int k=0;k<obj.h;k++){
                res(i,j)+=arr[i][k]*obj.arr[k][j];
            }
        }
    }
    // std::cout<<"Done"<<std::endl;
    return res;
}
Vec3f Matrix::operator*(Vec3f & obj)
{
    if (w!=3){
        throw std::invalid_argument("received non-matching col dimension");
    }
    Vec3f res=Vec3f();
    for (int i=0;i<h;i++){
        for(int k=0;k<3;k++){
            res[i]+=arr[i][k]*obj[k];
        }
    }
    // std::cout<<"Done"<<std::endl;
    return res;
}
Matrix::~Matrix(){
    for(int i=0;i<h;i++){
        delete[] arr[i];
    }
    delete[] arr;
}

void Matrix::set_col(int col, Vec3f uv){
    arr[0][col]=uv[0];
    arr[1][col]=uv[1];
}