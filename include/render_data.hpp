#pragma once
#include "common.hpp"

#include "material.hpp"
#include "mesh.hpp"

void setTexParameter(unsigned int tex_type);

struct TextureHandler {
    unsigned int texture,
                 texDimensions,
                  emptyTexturesCount,
                  texturesCount = 0,

                  // Pointers
                  *materialIndex,
                  *emptyTextures,
                  *texUtilitary;

    unsigned int addTex(unsigned char *localBuffer);
    void rmTex(unsigned int index);
    TextureHandler() = default;
};

struct VadiaMeshes {
    unsigned int verticesCount = 0,
                 verticesIndexCount = 0,
                 mesh_objects = -1;
};


class RenderData {
public:
    int delayCounter = -1;
    TextureHandler textureHandlers[TEXTURE_HANDLERS_COUNT];

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
        VadiaMeshes* meshes;
        unsigned int meshes_count = 0,

                     vertices_count = 0,
                    *vertices_offsets,

                     vertices_index_count = 0,
                    *vertices_index,
                    *vertices_index_offsets,

                    texture_vertices_count = 0,
                    *texture_vertices_offsets,

                    texture_vertices_index_count = 0,
                    *texture_vertices_index,
                    *texture_vertices_index_offsets;
        float *vertices,*texture_vertices;
        void init();
        void freeData();
    } compacted_meshes;


    Material *materials;

    unsigned int texUtilitary,
                 ebo,
                 vao,
                 vbo,
                 ssbos[SSBOS_COUNT],
                 materialsCount = 0;


    void update();

    TextureLocation addTexToHandler(char *path,bool toProcess=1);

    unsigned int allocMaterial(MaterialGenData data);
    void freeMaterial(unsigned int index);

    void addMesh(MeshGenData genData);
    void removeMesh(unsigned int index);

    RenderData();

    void freeData();
};
