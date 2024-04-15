#include "matrix.h"
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
static Matrix Matrix::identity(int size=4){
    Matrix m = Matrix(size,size);
    for(int i=0;i<size;++i){
        m[i][i]=1;
    }
    return m;
} 
float* Matrix::operator[](int index){
    return arr[index];
}
Matrix Matrix::operator*(Matrix const& obj)//dot product
{
    Matrix m=Matrix(w,obj.h);
    for (int i=0;i<w;i++){
        for(int j=0;j<obj.h;j++){
            for(int k=0;k<obj.w;k++){
                m[i][j]+=arr[i][k]*obj.arr[k][j];
            }
        }
    }
    return m;
}
Matrix::~Matrix(){
    for(int i=0;i<h;i++){
        delete[] arr[i];
    }
    delete[] arr;
}