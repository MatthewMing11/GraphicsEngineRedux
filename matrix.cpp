#include "matrix.h"
#include "vec3f.h"
#include <iostream>

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
    Matrix m=Matrix(1,3);
    for(int i=0;i<3;i++){
        m[0][i]=vec[i];
    }
}
Matrix Matrix::identity(int size){
    Matrix m = Matrix(size,size);
    for(int i=0;i<size;++i){
        m[i][i]=1;
    }
    return m;
} 
float* Matrix::operator[](int index){
    return arr[index];
}
Matrix Matrix::operator*(Matrix const& obj)//matrix multiplication
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
Matrix::~Matrix(){
    for(int i=0;i<h;i++){
        delete[] arr[i];
    }
    delete[] arr;
}