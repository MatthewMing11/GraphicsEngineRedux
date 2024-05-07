#pragma once
#include "vec3f.h"
class Matrix{
    private:
        float **arr;
        int w;
        int h;
    public:
        Matrix();
        Matrix(int row,int col);
        Matrix(Vec3f vec);
        Matrix(const Matrix& m);//copy constructor
        Matrix& operator=(const Matrix& m);//assignment operator
        static Matrix identity(int size=4);
        float& operator()(int x, int y);
        const float& operator()(int x, int y) const; 
        Matrix operator*(Matrix const& obj);//matrix multiplication
        void set_col(int col, Vec3f uv);
        ~Matrix();
};