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
                verticesIndexCount = 0;
};

struct VadiaGenMeshes {
    unsigned int verticesCount = 0,
                 verticesIndexCount = 0,
                *verticesIndex;
    float *vertices;
};

class RenderData {
public:
    char *flags;

    int delayCounter = -1;
    TextureHandler textureHandlers[TEXTURE_HANDLERS_COUNT];

    struct Compacted_Meshes{
        VadiaGenMeshes* meshes;
        unsigned int meshesCount = 0,
                     verticesCount = 0,
                     verticesIndexCount = 0,
                    *verticesIndex,
                    *verticesIndexOffsets,//[0] = 0,[1] = v
                    *meshObjects;
        float *vertices;
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
