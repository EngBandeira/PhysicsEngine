#pragma once

#include "material.hpp"
#include "shader.hpp"

class DrawGroup {
    public:
    unsigned int *objects, objects_count = 0, shaderProgram, vertices_index_count = 0, texture_vertices_index_count = 0;
    float *matrices;
    unsigned int ebo, matrices_ssbo, vertices_offset_ssbo, texture_vertices_index_ssbo, texture_vertices_offset_ssbo;
    Shader shader;
    Material material;
    unsigned int addObject(unsigned int object);

    DrawGroup(){}
    void init(const char *vertex_shaders, const char *geometry_shaders, const char *fragment_shaders);
    void update();
    void freeData();
};
