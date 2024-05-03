#include "model.h"
#include "matrix.h"
#include "vec3f.h"
#include "types.h"
#include <iostream>

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
            if (token!=""){//some lines have multiple whitespaces so ignore degenerate cases
                tokens.push_back(token);
            }
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
            std::string facedata;
            std::vector<int> vertexdata;
            std::vector<int> uvdata;
            std::vector<int> normaldata;
            int fSlash,sSlash;
            for(int i =1;i<4;i++){
                facedata=tokens[i];
                fSlash=facedata.find("/");
                if(fSlash==std::string::npos){
                    vertexdata.push_back(std::stoi(facedata)-1);//subtracting here so i don't have to subtract later
                    continue;
                }
                vertexdata.push_back(std::stoi(facedata.substr(0,fSlash))-1);
                facedata=facedata.substr(fSlash+1);
                sSlash=facedata.find("/");
                if(sSlash==std::string::npos){
                    uvdata.push_back(std::stoi(facedata)-1);
                    continue;
                }
                std::string optField=facedata.substr(0,sSlash);
                if (optField.size()!=0){
                    uvdata.push_back(std::stoi(facedata.substr(0,sSlash))-1);
                }
                facedata=facedata.substr(sSlash+1);
                normaldata.push_back(std::stoi(facedata)-1);
            }
            struct Face data={vertexdata,uvdata,normaldata};
            faces.push_back(data);
        }
        else if(tokens[0]=="vn"){
            Vec3f data = Vec3f(std::stof(tokens[1]),std::stof(tokens[2]),std::stof(tokens[3]));
            normals.push_back(data);
        }
        else if(tokens[0]=="vt"){
            Vec3f data = Vec3f(std::stof(tokens[1]),std::stof(tokens[2]),std::stof(tokens[3]));
            uvs.push_back(data);
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
    Matrix res = Matrix(4,1);
    res(0,0)=vertices[faces[iface].points[nthvert]].x;
    res(1,0)=vertices[faces[iface].points[nthvert]].y;
    res(2,0)=vertices[faces[iface].points[nthvert]].z;
    res(3,0)=1;
    return res;
}
int Model::nfaces(){
    return faces.size();
}