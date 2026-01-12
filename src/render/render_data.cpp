#include "common.hpp"
#include "lamp.hpp"
#include "material.hpp"
#include "model.hpp"
#include "log.hpp"
#include "render/render_data.hpp"
#include <cstdlib>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>

#include "vendor/glad/glad.h"//GLAD Always upper than GLFW
#include <GLFW/glfw3.h>

#include "vendor/stb_image/stb_image.h"

#include "utils.hpp"


void setTexParameter(){
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, LEVEL);
}

void MeshRenderData::init() {
    vertices = (float *) malloc(0);
    matrices = (float *) malloc(0);
    textureVertices = (float *) malloc(0);
    normalVertices = (float *) malloc(0);

    verticesIndex = (unsigned int *) malloc(0);
    modelParent = (unsigned int *) malloc(0);
    textureVerticesIndex = (unsigned int *) malloc(0);
    normalVerticesIndex = (unsigned int *) malloc(0);
    modelMaterial = (unsigned int *) malloc(0);
}

void MeshRenderData::freeData() {
    free(vertices);
    free(matrices);
    free(textureVertices);
    free(normalVertices);
    free(verticesIndex);
    free(modelParent);
    free(textureVerticesIndex);
    free(normalVerticesIndex);
    free(modelMaterial);
}

RenderData::RenderData() {
    for( int i = 0; i < LAYERS_COUNT; i++ ) {
        layers[i].init();
    }

    for( unsigned int i = 0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        textureHandlers[i].materialIndex = (unsigned int *) malloc(0);
        textureHandlers[i].texDimensions = pow(2, i + 6);
        textureHandlers[i].emptyTextures = (unsigned int *) malloc(0);
        textureHandlers[i].emptyTexturesCount = 0;
    }
    materials = (Material *) malloc(0);
    lamps = (Lamp*) malloc(0);
}

void RenderData::freeData() {
    for( unsigned int i =0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        free(textureHandlers[i].materialIndex);
        free(textureHandlers[i].emptyTextures);
    }

    for( int i = 0; i < LAYERS_COUNT; i++ ) {
        for( Model m: layers[i].models ) m.mesh.deleteMesh();
        layers[i].freeData();
    }

    free(materials);
    free(lamps);
}

glm::mat4 *RenderData::getNMatrix(unsigned short index, unsigned int layerIndex){
    return (glm::mat4*) (layers[layerIndex].matrices + 16*index);
}

void RenderData::scaleModel(glm::vec3 scale, unsigned short index, unsigned int layerIndex) {
    layers[layerIndex].models[index].scale(scale);

    *getNMatrix(index, layerIndex) = layers[layerIndex].models[index].matrix;
    *flags = *flags | MATRICES_CHANGE_FLAG;
}

void RenderData::scaleModel(float scale, unsigned short index, unsigned int layerIndex) {
    scaleModel(glm::vec3(scale), index, layerIndex);
}

void RenderData::translateModel(glm::vec3 translation, unsigned short index, unsigned int layerIndex) {
    layers[layerIndex].models[index].translate(translation);

    *getNMatrix(index, layerIndex) = layers[layerIndex].models[index].matrix;
    *flags = *flags | MATRICES_CHANGE_FLAG;
}

void RenderData::rotateModel(glm::vec3 rotation, unsigned short index, unsigned int layerIndex) {
    layers[layerIndex].models[index].rotate(rotation);

    *getNMatrix(index, layerIndex) = layers[layerIndex].models[index].matrix;
    *flags = *flags | MATRICES_CHANGE_FLAG;
}

void RenderData::setAngleModel(glm::vec3 angle, unsigned short index, unsigned int layerIndex) {
    layers[layerIndex].models[index].setAngle(angle);

    *getNMatrix(index, layerIndex) = layers[layerIndex].models[index].matrix;
    *flags = *flags | MATRICES_CHANGE_FLAG;
}

void RenderData::positionateModel(glm::vec3 position, unsigned short index, unsigned int layerIndex) {
    layers[layerIndex].models[index].positionate(position);

    *getNMatrix(index, layerIndex) = layers[layerIndex].models[index].matrix;
    *flags = *flags | MATRICES_CHANGE_FLAG;
}

glm::vec4 RenderData::getPositionOfModel(unsigned short index, unsigned int layerIndex) {
    return *getNMatrix(index, layerIndex) * glm::vec4(0, 0, 0, 1);
}

unsigned int RenderData::pushModels(MeshGenData mesh, unsigned int materialIndex, unsigned int layerIndex) {
    *flags = *flags | MODELS_CHANGE_FLAG;
    delayCounter = 0;
    layers[layerIndex].models.push_back(Model(Mesh(mesh), materialIndex));
    return layers[layerIndex].models.size() - 1;
}

unsigned int RenderData::pushModels(MeshGenData mesh, MaterialGenData material, unsigned int layerIndex) {
    return pushModels(mesh, allocMaterial(material), layerIndex);
}

unsigned int RenderData::pushModels(MeshGenData mesh, unsigned int layerIndex) {
    return pushModels(mesh, 0, layerIndex);
}

void RenderData::popModels(unsigned int index, unsigned int layerIndex) {
    if( index >= layers[layerIndex].models.size() ) logger.sendError("Render::popModels :index > models.size()\n", 1);

    *flags = *flags | MODELS_CHANGE_FLAG;
    layers[layerIndex].models.erase(layers[layerIndex].models.begin() + index);
}


unsigned int RenderData::allocMaterial(MaterialGenData data) {
    materialsCount++;
    materials = (Material*) realloc(materials, sizeof(Material) * materialsCount);
    Material *mater = materials + materialsCount - 1;

    mater->type = data.type;
    mater->bm   = data.bm;
    mater->Ni   = data.Ni;
    mater->d    = data.d;

    for( int i = 0; i < 3; i++ ) {
        mater->K[3 * i]     = data.K[i].x;
        mater->K[3 * i + 1] = data.K[i].y;
        mater->K[3 * i + 2] = data.K[i].z;
    }
    for( int i = 0; i < 4; i++ ) {
        if( data.type == TEXTURE && data.maps[i] != nullptr ) mater->maps[i] = addTexToHandler(data.maps[i]);
        else mater->maps[i] = TextureLocation {.handler = -1};
    }

    *flags = *flags | MATERIAL_CHANGE_FLAG;
    return materialsCount - 1;
}

void RenderData::freeMaterial(unsigned int index) {
    Material *newMat = (Material*) malloc(sizeof(Material) * (materialsCount - 1));

    if(index < 0){
        logger.sendError("freeMaterial: index < 0", 0);
        return;
    }
    if(index >= materialsCount){
        logger.sendError("freeMaterial: index >= materialsCount", 0);
        return;
    }
    memcpy(newMat, materials, index);
    memcpy(newMat + index + 1, materials + index + 1, materialsCount - 1 - index);

    free(materials);

    materials = newMat;
    materialsCount--;

    *flags = *flags | MATERIAL_CHANGE_FLAG;
}

unsigned int RenderData::allocLamp(LampsGenData data) {
    lampsCount++;
    lamps = (Lamp*) realloc(lamps, sizeof(Lamp) * lampsCount);
    Lamp *lamp = lamps + lampsCount - 1;

    lamp->position = data.position;


    *flags = *flags | LAMPS_CHANGE_FLAG;
    return lampsCount - 1;
};

void RenderData::freeLamp(unsigned int index) {
    Lamp *newLamps = (Lamp*) malloc(sizeof(Lamp) * (lampsCount - 1));

    if(index < 0){
        logger.sendError("freeLamp: index < 0", 0);
        return;
    }
    if(index >= lampsCount){
        logger.sendError("freeLamp: index >= lampsCount", 0);
        return;
    }
    memcpy(newLamps, lamps, index);
    memcpy(newLamps + index + 1, lamps + index + 1, lampsCount - 1 - index);

    free(lamps);

    lamps = newLamps;
    lampsCount--;

    *flags = *flags | LAMPS_CHANGE_FLAG;
}



// WW:widht
// HH:height
// BB: bpp
//0xFFRAWIMGWWHHBB
//0123
TextureLocation RenderData::addTexToHandler(char *localPath, bool toProcess) {
    char *path = localPath;
    if( toProcess ) {
        int i = 0, k;

        while( path[i] != 0 ) {
            if( path[i] == '.' ) k = i;
            i++;
        }

        char *newPath = (char*) malloc(k + RAW_TEX_EXTENSION_LENGHT + 2); //path + '.' + RAW_TEX_EXTENSION + '\0'
        memcpy(newPath, path, k+1);
        for( int j = 0; j < RAW_TEX_EXTENSION_LENGHT; j++ ) {
            newPath[k + 1 + j] = RAW_TEX_EXTENSION[j];
        }

        newPath[k + 1 + RAW_TEX_EXTENSION_LENGHT] = 0;
        if( access(newPath, F_OK) ) {
            int width, height, bpp;
            stbi_set_flip_vertically_on_load (1);
            //localBuffeSize = 4 * width * width
            unsigned char *localBuffer = stbi_load(path, &width,  &height, &bpp, 4);

            if(localPath == nullptr) logger.sendError("image do not exist", 1);
            if(width != height) logger.sendError("Width != Height", 1);
            if(width > 4096) logger.sendError("Width, Height > 4096", 1);


            path = newPath;
            FILE *file = fopen(path, "wb");
            char c[14];//13 is the lenght of 0xFFRAWIMGWWHHBB
            sprintf(c, "\xffRAWIMG");
            fwrite(c, sizeof(char), 7, file);
            char f[6];
            f[0] = (unsigned char)width;
            f[1] = (unsigned char)(width >> 8);
            f[2] = (unsigned char)height;
            f[3] = (unsigned char)(height >> 8);
            f[4] = (unsigned char)bpp;
            f[5] = (unsigned char)(bpp >> 8);
            fwrite(f, sizeof(char), 6, file);

            fwrite(localBuffer, 1, 4 * width * width, file);
            fclose(file);
        }
        else {
            path = newPath;
            // free(newPath);
        }
    }


    // i - k - 1;
    // stbi_set_flip_vertically_on_load (1);
    unsigned int lenght;
    unsigned char* localBuffer = (unsigned char*)utils.readFile(path, &lenght,"rb");
    if(! utils.matchPairs(localBuffer, 0, (unsigned char[]){(unsigned char)'\xff','R','A','W','I','M','G'}) ) {
        logger.sendError("addTexToHandler, file of wrong type");
        return TextureLocation();
    }

    unsigned short width = *((unsigned short*)(localBuffer + 7));
    // unsigned short height = *((unsigned short*)(localBuffer + 9));
    // unsigned short bpp = *((unsigned short*)(localBuffer + 11));


    int handlerIndex = (unsigned char)floor(log2(width)-6);
    unsigned int index = textureHandlers[handlerIndex].addTex((unsigned char*)localBuffer+13);
    free(localBuffer);

    if( toProcess ) free(path);

    return TextureLocation {
        .handler = handlerIndex,
        .index = index
    };
}

unsigned int TextureHandler::addTex(unsigned char *localBuffer){
    if(emptyTexturesCount) {
        unsigned int i = emptyTextures[0];
        glBindTexture(GL_TEXTURE_2D_ARRAY,texture);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, LEVEL, 0, 0,
                            i-1, texDimensions, texDimensions, 1, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);

        emptyTexturesCount--;
        emptyTextures = (unsigned int*)realloc(emptyTextures, emptyTexturesCount * sizeof(int));
        return i;
    }


    glBindTexture(GL_TEXTURE_2D_ARRAY, *texUtilitary);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,texDimensions,texDimensions,texturesCount+1,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);

    setTexParameter();


    glCopyImageSubData(texture,GL_TEXTURE_2D_ARRAY,LEVEL,0,0,0,*texUtilitary, GL_TEXTURE_2D_ARRAY, LEVEL,
                    0,0,0,texDimensions,texDimensions,texturesCount);

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, LEVEL, 0, 0,
                        texturesCount, texDimensions, texDimensions, 1, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);

    glBindTexture(GL_TEXTURE_2D_ARRAY,texture);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 , 0, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D_ARRAY,0);

    texturesCount++;

    unsigned int cache = texture;
    texture = *texUtilitary;
    *texUtilitary = cache;

    return texturesCount-1;
}

void TextureHandler::rmTex(unsigned int index) {
    emptyTexturesCount++;

    if( emptyTexturesCount > MAX_EMPTY_TEXTURES_COUNT
        || (emptyTexturesCount > MIN_EMPTY_TEXTURES_COUNT_TO_RST && emptyTexturesCount > texturesCount * MAX_RATIO_OF_EMPTY_TEXTURES)
    ) return;

    emptyTextures = (unsigned int*)realloc(emptyTextures, emptyTexturesCount);
    emptyTextures[emptyTexturesCount-1] = index;
}
