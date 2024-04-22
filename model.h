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
        float scaleX;
        float scaleY;
        float xMin;
        float yMin;
    public:
        Model(char *path,int width,int height);
        Vec3f v_normal(int iface, int nthvert);
        Matrix vert(int iface, int nthvert);
        int nfaces();
};