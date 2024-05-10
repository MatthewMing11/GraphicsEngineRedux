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
#include "tgaimage.h"

class Model{
    private:
        std::vector<Vertex> vertices;
        std::vector<Face> faces;
        std::vector<Vec3f> normals;
        std::vector<Vec3f> uvs;
        TGAImage diffusemap_;
        TGAImage normalmap_;
        TGAImage specularmap_;
        void load_texture(std::string filename, const char *suffix, TGAImage &img);
        float scaleX;
        float scaleY;
        float xMin;
        float yMin;
    public:
        Model(char *path,int width,int height);
        Vec3f v_normal(int iface, int nthvert);
        Vec3f normal(Vec3f uvf);
        Vertex vert(int iface, int nthvert);
        Vec3f uv(int iface,int nthvert);
        uint32_t diffuse(Vec3f uv);
        float specular(Vec3f uvf);
        int nfaces();
};