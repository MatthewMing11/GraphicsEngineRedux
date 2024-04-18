#pragma once
#include <string>
#include <vector>
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
    int points[3];
};
struct Normal{
    float x;
    float y;
    float z;
}

class Model{
    private:
        vector<Vertex> vertices;
        vector<Face> faces;
        vector<Normal> normals;
        float scaleX;
        float scaleY;
    public:
        Model(char *path);
        Vec3f v_normal(int iface, int nthvert);
        Matrix vert(int iface, int nthvert);
        int nfaces();
};