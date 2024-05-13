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
    return res;
}
Vec3f Matrix::operator*(Vec3f const& obj)
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
    return res;
}
Matrix Matrix::invert_transpose(){
    //inverse calculation from mesa implementation of glu library
    float inv[4][4], det;
    float m[16]={arr[0][0],arr[0][1],arr[0][2],arr[0][3],
                    arr[1][0],arr[1][1],arr[1][2],arr[1][3],
                    arr[2][0],arr[2][1],arr[2][2],arr[2][3],
                    arr[3][0],arr[3][1],arr[3][2],arr[3][3]};
    inv[0][0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[1][0] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[2][0] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[3][0] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[0][1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[1][1] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[2][1] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[3][1] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[0][2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[1][2] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[2][2] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[3][2] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[0][3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[1][3] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[2][3] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[3][3] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0][0] + m[1] * inv[1][0] + m[2] * inv[2][0] + m[3] * inv[3][0];

    if (det == 0)
        throw std::invalid_argument("received non-invertible matrix");

    det = 1.0 / det;

    Matrix mat = Matrix(4,4);
    for (int i = 0; i < 4; i++){
        for(int j=0;j<4;j++){
            mat(i,j) = inv[i][j] * det;
        }
    }
    //tranpose portion
    float temp;
    for(int i=0;i<4;i++){
        for(int j=i+1;j<4;j++){
            temp=mat(i,j);
            mat(i,j)=mat(j,i);
            mat(j,i)=temp;
        }
    }
    return mat;
}
Matrix Matrix::transpose(){
    Matrix res = Matrix(w,h);
    for (int i = 0; i < w; i++){
        for(int j=0;j<h;j++){
            res(i,j) = arr[j][i];
        }
    }
    return res;
}
Matrix Matrix::invert(){
    Matrix m=Matrix(3,3);
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            m(i,j)=arr[i][j];
        }
    }
    double det = m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) -
             m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
             m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));

    double invdet = 1 / det;

    Matrix minv=Matrix(3,3); // inverse of matrix m
    minv(0, 0) = (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) * invdet;
    minv(0, 1) = (m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2)) * invdet;
    minv(0, 2) = (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * invdet;
    minv(1, 0) = (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) * invdet;
    minv(1, 1) = (m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0)) * invdet;
    minv(1, 2) = (m(1, 0) * m(0, 2) - m(0, 0) * m(1, 2)) * invdet;
    minv(2, 0) = (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)) * invdet;
    minv(2, 1) = (m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1)) * invdet;
    minv(2, 2) = (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) * invdet;
    return  minv;
}
Vec3f Matrix::col(int col){
    return Vec3f(arr[0][col],arr[1][col],arr[2][col]);
}
Matrix::~Matrix(){
    for(int i=0;i<h;i++){
        delete[] arr[i];
    }
    delete[] arr;
}

void Matrix::set_col(int col, Vec3f uv){
    for(int i=0;i<std::min(h,3);i++){
        arr[i][col]=uv[i];
    }
}