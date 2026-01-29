#pragma once

class DrawGroup {
    public:
    unsigned int *objects, objects_count = 0, vertices_index_count = 0, texture_vertices_index_count = 0;
    float *matrices;
    unsigned int ebo, matrices_ssbo, vertices_offset_ssbo, texture_vertices_index_ssbo, texture_vertices_offset_ssbo;
    unsigned int program;
    unsigned int material;
    unsigned int addObject(unsigned int object);

    DrawGroup(){}
    void init();
    void update();
    void freeData();
};
