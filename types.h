#pragma once
#include<vector>
struct Vertex{
    float x;
    float y;
    float z;
};
struct Face{
    std::vector<int> points;
    std::vector<int> uvs;
    std::vector<int> normals;
};