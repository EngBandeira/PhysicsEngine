#pragma once

#include "log.hpp"


struct MeshGenData {
    char *path;
};


class Mesh {
    public:
    // Ponto[verticeCount/3] =
    // {vertices[verticesCount],vertices[verticesCount+1],vertices[verticesCount+2]}
    // Ponto[textureVerticesCount/2] =
    // {textureVertices[textureVerticesCount],textureVertices[textureVerticesCount+1]}
    unsigned int oCount = 0;
    char *mtl;
    char **o;
    unsigned int verticesCount = 0, verticesIndexCount = 0, textureVerticesCount=0,
                 textureVerticesIndexCount=0,normalVerticesCount=0,verticesOffsetIndex= 0;// normalIndexCount = nOfPrimitives = verticesIndexCount /3
    float *vertices,*textureVertices,*normalVertices;
    unsigned int *verticesIndex,*textureVerticesIndex,*normalVerticesIndex;
    const char *meshPath;
    void deleteMesh();
    Mesh(){};
    Mesh(MeshGenData genData);

    ~Mesh();
};
