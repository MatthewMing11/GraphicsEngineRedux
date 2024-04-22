#include "model.h"
#include "matrix.h"
#include "vec3f.h"
#include "types.h"

Model::Model(char* path,int width,int height)
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
        else if (tokens[0]=="f"){
            struct Face data={{std::stoi(tokens[1])-1,std::stoi(tokens[2])-1,std::stoi(tokens[3])-1}};//subtracting here so i don't have to subtract later
            faces.push_back(data);
        }
        else if(tokens[0]=="vn"){
            Vec3f data = Vec3f(std::stof(tokens[1]),std::stof(tokens[2]),std::stof(tokens[3]));
            normals.push_back(data);
        }
    }
    this->faces=faces;
    this->vertices=vertices;
    scaleX=(xMax-xMin)/width;
    scaleY=(yMax-yMin)/height;
    this->xMin=xMin;
    this->yMin=yMin;
    this->scaleX=scaleX;
    this->scaleY=scaleY;
    if (normals.size()==0){// if normals are not given in file
    //code inspired by https://computergraphics.stackexchange.com/questions/4031/programmatically-generating-vertex-normals
        for(int i=0;i<vertices.size();i++){
            Vec3f n = Vec3f(0.f,0.f,0.f);
            normals.push_back(n);
        }
        for(int i=0;i<faces.size();i++){
            Vec3f A = Vec3f(vertices[faces[i].points[0]].x,vertices[faces[i].points[0]].y,vertices[faces[i].points[0]].z);
            Vec3f B = Vec3f(vertices[faces[i].points[1]].x,vertices[faces[i].points[1]].y,vertices[faces[i].points[1]].z);
            Vec3f C = Vec3f(vertices[faces[i].points[2]].x,vertices[faces[i].points[2]].y,vertices[faces[i].points[2]].z);
            Vec3f AB=B-A;
            Vec3f AC=C-A;
            Vec3f ABxAC=AB^AC;
            normals[faces[i].points[0]]+=ABxAC;
            normals[faces[i].points[1]]+=ABxAC;
            normals[faces[i].points[2]]+=ABxAC;

        }
        for(int i=0;i<normals.size();i++){
            normals[i]=normals[i].normalize();
        }
    }
}
Vec3f Model::v_normal(int iface, int nthvert){
    Vec3f v_normal = normals[faces[iface].points[nthvert]];
    return v_normal;
}
Matrix Model::vert(int iface, int nthvert){
    Matrix res = Matrix(1,4);
    res[0][0]=static_cast<int>((vertices[faces[iface].points[nthvert]-1].x-xMin)/scaleX);
    res[0][1]=static_cast<int>((vertices[faces[iface].points[nthvert]-1].y-yMin)/scaleY);
    res[0][2]=0;
    res[0][3]=1;
    return res;
}
int Model::nfaces(){
    return faces.size();
}