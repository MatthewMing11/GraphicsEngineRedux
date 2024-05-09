#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include "vec3f.h"
#include "matrix.h"
#include "types.h"

class Model{
    private:
        std::vector<Vertex> vertices;
        std::vector<Face> faces;
        std::vector<Vec3f> normals;
        std::vector<Vec3f> uvs;
        float scaleX;
        float scaleY;
        float xMin;
        float yMin;
    public:
        Model(char *path,int width,int height);
        Vec3f v_normal(int iface, int nthvert);
        Vertex vert(int iface, int nthvert);
        Vec3f uv(int iface,int nthvert);
        Vec3f diffuse(Vec3f uv);
        int nfaces();
};