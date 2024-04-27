#pragma once
#include "vec3f.h"
class Matrix{
    private:
        float **arr;
        int w;
        int h;
    public:
        Matrix()=default;
        Matrix(int row,int col);
        Matrix(Vec3f vec);
        static Matrix identity(int size=4);
        float& operator()(int x, int y);
        const float& operator()(int x, int y) const; 
        Matrix operator*(Matrix const& obj);//matrix multiplication
        ~Matrix();
};