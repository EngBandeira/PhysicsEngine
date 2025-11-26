#pragma once

#include <vector>

#include "utils.hpp"

struct Index
{
    unsigned int v[3];
    inline Index (unsigned int v_[3])
    {
        v[0] = v_[0];
        v[1] = v_[1];
        v[2] = v_[2];
    }
};
class Mesh
{
  public:
    unsigned int verticesCount, textureVerticesCount, verticesIndexCount,
        textureIndexCount, *verticesIndex, *textureIndex;
    float *textureVertices, *vertices;

    Mesh ();
    Mesh (const char *localPath);
    ~Mesh ();
};

class Model
{
    Mesh mesh;
    
};