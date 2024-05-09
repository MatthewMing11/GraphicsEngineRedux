#include "vec3f.h"
#include <cmath>

Vec3f::Vec3f(){
    arr[0]=0;
    arr[1]=0;
    arr[2]=0;
}
Vec3f::Vec3f(float x,float y, float z)
{
    arr[0]=x;
    arr[1]=y;
    arr[2]=z;
}
Vec3f Vec3f::operator+(Vec3f const& obj)
{
    Vec3f res=Vec3f(arr[0]+obj.arr[0],arr[1]+obj.arr[1],arr[2]+obj.arr[2]);
    return res;
}
Vec3f Vec3f::operator-(Vec3f const& obj)
{
    Vec3f res=Vec3f(arr[0]-obj.arr[0],arr[1]-obj.arr[1],arr[2]-obj.arr[2]);
    return res;
}
float Vec3f::operator*(Vec3f const& obj)//dot product
{
    return arr[0]*obj.arr[0]+arr[1]*obj.arr[1]+arr[2]*obj.arr[2];
}
Vec3f Vec3f::operator*(float const& k)//scalar multiplication
{
    Vec3f res=Vec3f(arr[0]*k,arr[1]*k,arr[2]*k);
    return res;
}
Vec3f Vec3f::operator^(Vec3f const& obj)//cross product
{
    Vec3f res=Vec3f(arr[1]*obj.arr[2]-arr[2]*obj.arr[1],arr[2]*obj.arr[0]-arr[0]*obj.arr[2],arr[0]*obj.arr[1]-arr[1]*obj.arr[0]);
    return res;
}
float& Vec3f::operator[](int index){
    return arr[index];
}

Vec3f& Vec3f::operator+=(Vec3f const& obj){
    this->arr[0]+=obj.arr[0];
    this->arr[1]+=obj.arr[1];
    this->arr[2]+=obj.arr[2];
    return *this;
}
float Vec3f::norm()
{
    return std::sqrt(std::pow(arr[0],2)+std::pow(arr[1],2)+std::pow(arr[2],2));
}
Vec3f Vec3f::normalize()//modifies itself and returns for convenience
{
    if(arr[0]==0 && arr[1]==0 && arr[2]==0)//degenerate case zero vector
    {
        return *this;
    }
    float mag = this->norm();
    arr[0]=arr[0]/mag;
    arr[1]=arr[1]/mag;
    arr[2]=arr[2]/mag;
    Vec3f res=Vec3f(arr[0],arr[1],arr[2]);
    return res;
}
