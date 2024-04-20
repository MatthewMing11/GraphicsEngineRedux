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
        std::vector<Normal> normals;
        float scaleX;
        float scaleY;
    public:
        Model(char *path);
        Vec3f v_normal(int iface, int nthvert);
        Matrix vert(int iface, int nthvert);
        int nfaces();
};