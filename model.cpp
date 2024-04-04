#include "model.h"

// void Mesh::setupMesh()
// {

// }
Mesh::Mesh(vector<Vertex> vertices){
    this->vertices = vertices;
    setupMesh();
}
// Model::loadModel(string path)
// {

// }
void Mesh::Draw(Shader &shader){
    for(int i=0;i<meshes.size();i++){
        meshes[i].Draw(shader);
    }
}

void Model::Draw(Shader &shader){
    for(int i=0;i<meshes.size();i++){
        meshes[i].Draw(shader);
    }
}