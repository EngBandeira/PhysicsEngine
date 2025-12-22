#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include <vector>

#include "texture.hpp"
#include "utils.hpp"





class Mesh {
    public:
    // Ponto[verticeCount/3] =
    // {vertices[verticesCount],vertices[verticesCount+1],vertices[verticesCount+2]}
    // Ponto[textureVerticesCount/2] =
    // {textureVertices[textureVerticesCount],textureVertices[textureVerticesCount+1]}
    //
    //
    glm::mat4 matrix;
    unsigned int verticesCount = 0, verticesIndexCount = 0, textureVerticesCount=0,
                 textureVerticesIndexCount=0,normalVecCount=0,normalIndexCount=0,verticesOffsetIndex= 0;
    float *vertices,*textureVertices,*normalVec;
    int renderFlags = 0;
    unsigned int *verticesIndex,*textureVerticesIndex,*normalIndex;
    const char *meshPath;
    void deleteMesh();
    Mesh(){};
    Mesh(const char *meshPath);
    ~Mesh();
    // ~Mesh();
};

class Model
{
    public:
    Mesh mesh;
    Texture tex;
    Model(){};
    Model(const char *meshPath,const char *texPath):mesh(meshPath), tex(texPath){};
    ~Model(){};
};
