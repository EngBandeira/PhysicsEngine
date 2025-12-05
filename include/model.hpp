#pragma once

#include <vector>

#include "utils.hpp"


class Mesh {
    public:
    // Ponto[verticeCount/3] =
    // {vertices[verticesCount],vertices[verticesCount+1],vertices[verticesCount+2]}
    // Ponto[textureVerticesCount/2] =
    // {textureVertices[textureVerticesCount],textureVertices[textureVerticesCount+1]}
    unsigned int verticesCount, verticesIndexCount, textureVerticesCount,textureIndexCount;
    float *vertices, *textureVertices;
    unsigned int *verticesIndex, *textureIndex;
    Mesh();
    Mesh(const char *localPath);
    ~Mesh();
};

class Model {
  Mesh mesh;
};
