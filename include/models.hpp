#pragma once

#include <vector>

#include "utils.hpp"

struct Index
{
    unsigned int v[3];
    Index(    unsigned int v_[3]){
      v[0] = v_[0];
      v[1] = v_[1];
      v[2] = v_[2];
    }
};
    struct model
    {
        std::vector<vec3> vertex;
        std::vector<vec2> texture;
        std::vector<Index> vertexIndex, textureVertexIndex;

        model ();
        model (const char *localPath);
        ~model ();
    };
