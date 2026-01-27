#pragma once
#include "common.hpp"

#include "draw_group.hpp"
#include "material.hpp"
#include "mesh.hpp"

void setTexParameter(unsigned int tex_type);

struct Texture_Handler {
    unsigned int texture,
                 texDimensions,
                  emptyTexturesCount,
                  texturesCount = 0,

                  // Pointers
                  *materialIndex,
                  *emptyTextures;

    unsigned int add_tex(unsigned char *localBuffer);
    void rm_tex(unsigned int index);
    Texture_Handler() = default;
};

struct Vadia_Mesh {
    unsigned int vertices_count = 0,
                 vertices_index_count = 0,
                 texture_vertices_count = 0,
                 texture_vertices_index_count = 0,
                 mesh_objects = -1;
    Vadia_Mesh(){}
};


class Render_Data {
public:
    int delayCounter = -1;
    Texture_Handler textureHandlers[TEXTURE_HANDLERS_COUNT];

    // unsigned int verticesCount = 0,
    //              verticesIndexCount = 0,
    // //              textureVerticesCount = 0,
    // //              textureVerticesIndexCount = 0,
    // //              verticesOffsetIndex = 0, // normalIndexCount = nOfPrimitives = verticesIndexCount /3

    // //             // pointers
    //             *verticesIndex,
    // //             *textureVerticesIndex,

    // float *vertices,
    //       *textureVertices,

    struct Compacted_Meshes{
        Vadia_Mesh* meshes;
        DrawGroup *draw_groups;
        unsigned int meshes_count = 0,
                     draw_groups_count = 0,
                     vertices_count = 0,
                     vertices_index_count = 0,
                     texture_vertices_count = 0,
                     texture_vertices_index_count = 0,

                     *vertices_index,
                     *vertices_index_offsets,

                     *texture_vertices_index,
                     *texture_vertices_index_offsets;



        float *vertices, *texture_vertices;
        Compacted_Meshes(){}
        void init();
        void free_data();
    } compacted_meshes;



    Material *materials;

    unsigned int texUtilitary,
                 buffer_utilitary,
                 vao,
                 vbo,
                 ssbos[SSBOS_COUNT],
                 materialsCount = 0;

    unsigned int alloc_draw_group(const char *vertex_shaders, const char *geometry_shaders, const char *fragment_shaders);


    void update();

    TextureLocation add_tex_to_handler(char *path,bool toProcess=1);

    unsigned int alloc_material(MaterialGenData data);
    void free_material(unsigned int index);

    unsigned int add_mesh(MeshGenData genData);
    void remove_mesh(unsigned int index);

    void init();

    void free_data();
};
