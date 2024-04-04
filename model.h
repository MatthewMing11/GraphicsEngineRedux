#pragma once
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
};