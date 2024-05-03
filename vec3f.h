#pragma once
class Vec3f{//inspired by Vec3f java graphics class
    private:
        float arr[3];
    public:
        Vec3f();
        Vec3f(float x,float y, float z);
        Vec3f operator+(Vec3f const& obj);
        Vec3f operator-(Vec3f const& obj);
        float operator*(Vec3f const& obj);//dot product
        Vec3f operator^(Vec3f const& obj);//cross product
        float& operator[](int index);
        Vec3f& operator+=(Vec3f const& obj);
        float norm();
        Vec3f normalize();//modifies itself and returns for convenience
};