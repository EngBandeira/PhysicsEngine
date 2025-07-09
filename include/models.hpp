#pragma once

#include <vector>

#include "utils.hpp"

struct triangle
{
    unsigned int v[3];
    triangle(    unsigned int v_[3]){
      v[0] = v_[0];
      v[1] = v_[1];
      v[2] = v_[2];
    }
};
    struct model
    {
        std::vector<vec3> vertex;
        std::vector<vec2> texture;
        std::vector<triangle> trianglesVertex, trianglesTextureVertex;

        model ();
        model (const char *localPath);
        ~model ();
    };
