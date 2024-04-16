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
    int first;
    int second;
    int third;
};

class Model{
    private:
        vector<Vertex> vertices;
        vector<Face> faces;
        vector<Mesh> meshes;
    public:
        Model(char *path);
};