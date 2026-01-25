#pragma once

#include "game_object.hpp"
#include "material.hpp"
#include "shader.hpp"

class DrawGroup{
    public:
    unsigned int *objects, objects_count, shaderProgram, vertices_index_count;
    float *matrices;
    unsigned int ebo, matrices_ssbo, vertices_offset_ssbo;
    Shader shader;
    Material material;
    unsigned int addObject(unsigned int object);

    void init(Shader shader);
    void update();
    void freeData();
};
