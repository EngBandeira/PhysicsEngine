#pragma once

#include <vector>

#include "texture.hpp"
#include "utils.hpp"


class Mesh {
    public:
    // Ponto[verticeCount/3] =
    // {vertices[verticesCount],vertices[verticesCount+1],vertices[verticesCount+2]}
    // Ponto[textureVerticesCount/2] =
    // {textureVertices[textureVerticesCount],textureVertices[textureVerticesCount+1]}
    unsigned int verticesCount, verticesIndexCount, textureVerticesCount,textureIndexCount,normalVecCount;
    float *vertices, *textureVertices,*normalVec;
    unsigned int *verticesIndex, *textureIndex,*normalIndex;
    Mesh();
    Mesh(const char *meshPath);
    ~Mesh();
};

class Model
{
    public:
    Texture tex;
    Mesh mesh;
    Model(const char *meshPath,const char *texPath):tex(Texture(texPath)),mesh(Mesh(meshPath)){};
};
