#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

#include "texture.hpp"
#include "utils.hpp"


class Mesh {
    public:
    // Ponto[verticeCount/3] =
    // {vertices[verticesCount],vertices[verticesCount+1],vertices[verticesCount+2]}
    // Ponto[textureVerticesCount/2] =
    // {textureVertices[textureVerticesCount],textureVertices[textureVerticesCount+1]}
    glm::mat4 matrix;
    unsigned int verticesCount, verticesIndexCount;
    float *vertices;
    unsigned int *verticesIndex;
    Mesh(){};
    Mesh(const char *meshPath);
    // ~Mesh();
};

class Model
{
    public:
    Mesh mesh;
    Model(){};
    Model(const char *meshPath,const char *texPath):mesh(Mesh(meshPath)){};
};
