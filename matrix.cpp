#include "matrix.h"
#include "vec3f.h"
#include <iostream>

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
Matrix::Matrix(Vec3f vec){
    Matrix m= Matrix(1,3);
    for(int i=0;i<3;i++){
        m(0,i)=vec[i];
    }
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
Matrix::~Matrix(){
    for(int i=0;i<h;i++){
        delete[] arr[i];
    }
    delete[] arr;
}