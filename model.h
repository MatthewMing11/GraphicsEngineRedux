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
class Mesh{
    public:
        vector<Vertex> vertices;
        Mesh(vector<Vertex> vertices);
        void Draw(Shader &shader);
    private:
        void setupMesh();
};
class Model{
    public:
        Model(char *path)
        {
            loadModel(path);
        }
        void Draw(Shader &shader);
    private:
        vector<Mesh> meshes;
        string directory;
        void loadModel(string path);
        // void processNode();
        // Mesh processMesh();
        // vector<Texture> loadMaterialTextures();
};

// struct IShader{
//     virtual ~IShader();
//     virtual Vec3f vertex(int iface, int othvert)=0;
//     virtual bool fragment(Vec3f bar, uint32_t color)=0;
// };