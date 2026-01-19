#pragma once
#include "common.hpp"
#include "glm/glm.hpp"

#include "material.hpp"
#include "model.hpp"
#include "log.hpp"
#include "lamp.hpp"

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

struct MeshRenderData {
    std::vector<Model> models;

    unsigned int meshesCount = 0,
                 verticesCount = 0,
                 verticesIndexCount = 0,
                 textureVerticesCount = 0,
                 textureVerticesIndexCount = 0,
                 normalVerticesCount = 0, // normalIndexCount = nOfPrimitives = verticesIndexCount / 3
                 verticesIndexOffset = 0,
                 textureIndexOffset = 0,
                 normalIndexOffset = 0,

                 // Pointers
                 *verticesIndex,
                 *modelParent,
                 *textureVerticesIndex,
                 *normalVerticesIndex,
                 *modelMaterial,
                 *layerIndex,
                 *modelFlags;

    float *vertices,
          *matrices,
          *textureVertices,
          *normalVertices;

    unsigned int ebo,
                 vao,
                 vbos[VBO_COUNT],
                 ssbos[SSBO_PER_LAYER_COUNT];

    void init();
    void freeData();
};



class RenderData {
public:
    char *flags;

    int delayCounter = -1;

    TextureHandler textureHandlers[TEXTURE_HANDLERS_COUNT];
    Material *materials;
    Lamp *lamps;

    unsigned int texUtilitary,
                 materialsSSBO,
                 lampsSSBO,
                 materialsCount = 0,
                 lampsCount = 0;

    MeshRenderData layers[LAYERS_COUNT];

    // Methods

    TextureLocation addTexToHandler(char *path,bool toProcess=1);

    unsigned int allocMaterial(MaterialGenData data);
    void freeMaterial(unsigned int index);

    unsigned int allocLamp(LampsGenData data);
    void freeLamp(unsigned int index);

    unsigned int pushModels(MeshGenData mesh,unsigned int materialIndex, unsigned int layerIndex);
    unsigned int pushModels(MeshGenData mesh,MaterialGenData material, unsigned int layerIndex);
    unsigned int pushModels(MeshGenData mesh, unsigned int layerIndex);
    void popModels(unsigned int index,unsigned int layerIndex);

    void scaleModel(glm::vec3 scale, unsigned short index,unsigned int layerIndex);
    void scaleModel(float scale, unsigned short index,unsigned int layerIndex);
    void translateModel(glm::vec3 translation, unsigned short index,unsigned int layerIndex);
    void rotateModel(glm::vec3 rotation, unsigned short index,unsigned int layerIndex);
    void setAngleModel(glm::vec3 angle, unsigned short index,unsigned int layerIndex);
    void positionateModel(glm::vec3 position, unsigned short index,unsigned int layerIndex);

    glm::mat4 *getNMatrix(unsigned short index,unsigned int layerIndex);
    glm::vec4 getPositionOfModel(unsigned short index,unsigned int layerIndex);

    RenderData();

    void freeData();
};
