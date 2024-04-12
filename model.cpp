#include "model.h"
#include "matrix.h"
// void Mesh::setupMesh()
// {

// }
Mesh::Mesh(vector<Vertex> vertices){
    this->vertices = vertices;
    setupMesh();
}
Model::loadModel(string path)
{
    std::ifstream file(path);
    std::string line;
    std::vector<std::vector<std::string>> info;//for debugging
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    Vec3f light_dir=Vec3f(0,0,-1);
    float xMin=0,xMax=0,
        yMin=0,yMax=0,
        zMin=0,zMax=0;
    while(std::getline(file,line))
    {
        if(line.size()==0){//ignore blank lines
            continue;
        }
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        while(std::getline(iss,token,' '))//tokenizing line
        {
            tokens.push_back(token);
        }
        info.push_back(tokens);
        if (tokens[0]=="v"){
            Vertex data = {std::stof(tokens[1]),std::stof(tokens[2]),std::stof(tokens[3])};
            xMin=std::min(xMin,data.x);
            yMin=std::min(yMin,data.y);
            zMin=std::min(zMin,data.z);
            xMax=std::max(xMax,data.x);
            yMax=std::max(yMax,data.y);
            zMax=std::max(zMax,data.z);
            vertices.push_back(data);
        }
        if (tokens[0]=="f"){
            Face data={std::stoi(tokens[1]),std::stoi(tokens[2]),std::stoi(tokens[3])};
            faces.push_back(data);
        }
    }
}
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