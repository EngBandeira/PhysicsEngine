#pragma once

#include "common.hpp"
#include "mesh.hpp"



struct Vadia_Mesh {
    unsigned int vertices_count = 0,
                 index_count = 0,
                 texture_vertices_count = 0,
                 mesh_objects = -1;
    Vadia_Mesh(){}
};


class Render_Data {
public:
    int delayCounter = -1;

    struct Compacted_Meshes{
        Vadia_Mesh* meshes;
        unsigned int meshes_count = 0,
                     draw_groups_count = 0,
                     vertices_count = 0,
                     index_count = 0,
                     texture_vertices_count = 0,

                     *vertices_index,
                     *vertices_index_offsets,

                     *texture_vertices_index,
                     *texture_vertices_index_offsets;



        float *vertices, *texture_vertices;
        Compacted_Meshes(){}
        void init();
        void free_data();
    } compacted_meshes;





    unsigned int texUtilitary,
                 buffer_utilitary,
                 vao,
                 vbo,
                 ssbos[COMMON_SSBOS_COUNT],
                 materialsCount = 0;

    void update();

    unsigned int add_mesh(MeshGenData genData);
    void remove_mesh(unsigned int index);

    void init();

    void free_data();
};
