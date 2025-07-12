#pragma once

#include <vector>

#include "utils.hpp"

struct Index
{
    unsigned int v[3];
    Index (unsigned int v_[3])
    {
        v[0] = v_[0];
        v[1] = v_[1];
        v[2] = v_[2];
    }
};
class Model
{
    std::vector<vec3> vertex;
    std::vector<vec2> texture;
    std::vector<Index> vertexIndex, textureVertexIndex;

  public:
    float *exportVertices (unsigned int *count);
    float *exportTexture (unsigned int *count);
    unsigned int *exportVerticesIndex (unsigned int *count);
    unsigned int *exportTextureIndex (unsigned int *count);

    Model ();
    Model (const char *localPath);
    ~Model ();
};
