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
    load_texture(path, "_diffuse.tga", diffusemap_);
    // load_texture(path, "_nm.tga",      normalmap_);
    load_texture(path, "_nm_tangent.tga",      normalmap_);
    load_texture(path, "_spec.tga",    specularmap_);
}
Vec3f Model::v_normal(int iface, int nthvert){
    Vec3f v_normal = normals[faces[iface].points[nthvert]];
    return v_normal.normalize();
}
Vec3f Model::normal(Vec3f uvf){
    TGAColor c = normalmap_.get(uvf[0]*normalmap_.get_width(), uvf[1]*normalmap_.get_height());
    Vec3f res;
    for (int i=0; i<3; i++)
        res[2-i] = (float)c[i]/255.f*2.f - 1.f;
    return res;
}
Vertex Model::vert(int iface, int nthvert){
    return vertices[faces[iface].points[nthvert]];
}
Vec3f Model::uv(int iface,int nthvert){
    return uvs[faces[iface].uvs[nthvert]];
}
uint32_t Model::diffuse(Vec3f uv){
    TGAColor color = diffusemap_.get(uv[0]*diffusemap_.get_width(), uv[1]*diffusemap_.get_height());
    uint8_t R,G,B;
    R=color[2];
    G=color[1];
    B=color[0];
    return (R<<16)+(G<<8)+B;
}
float Model::specular(Vec3f uvf){
    return specularmap_.get(uvf[0]*specularmap_.get_width(), uvf[1]*specularmap_.get_height())[0]/1.f;;
}
int Model::nfaces(){
    return faces.size();
}
void Model::load_texture(std::string filename, const char *suffix, TGAImage &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot!=std::string::npos) {
        texfile = texfile.substr(0,dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}