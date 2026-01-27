#include "common.hpp"
#include "material.hpp"
#include "render_data.hpp"
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>

#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>

#include "utils.hpp"

void setTexParameter(unsigned int tex_type){
    glTexParameteri(tex_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(tex_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(tex_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex_type ,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex_type, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(tex_type, GL_TEXTURE_MAX_LEVEL, LEVEL);
}



void Render_Data::Compacted_Meshes::init(){
    meshes = (Vadia_Mesh*)malloc(0);
    draw_groups = (DrawGroup*)malloc(0);

    vertices_index = (unsigned int*)malloc(0);
    vertices_index_offsets = (unsigned int*)malloc(sizeof(int));
    vertices_index_offsets[0] = 0;

    texture_vertices_index = (unsigned int*)malloc(0);
    texture_vertices_index_offsets = (unsigned int*)malloc(sizeof(int));
    texture_vertices_index_offsets[0] = 0;


    vertices = (float*)malloc(0);
    texture_vertices = (float*)malloc(0);
}

void Render_Data::Compacted_Meshes::free_data() {

    for( unsigned int i = 0; i < draw_groups_count; i++ ) {
        draw_groups[i].freeData();
    }

    free(meshes);

    free(vertices);
    free(vertices_index);
    free(vertices_index_offsets);
    free(draw_groups);
}

void Render_Data::init() {
    compacted_meshes.init();

    for( unsigned int i = 0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        textureHandlers[i].materialIndex = (unsigned int *) malloc(0);
        textureHandlers[i].texDimensions = pow(2, i + 6);
        textureHandlers[i].emptyTextures = (unsigned int *) malloc(0);
        textureHandlers[i].emptyTexturesCount = 0;
    }
    materials = (Material *) malloc(0);


    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    // glGenBuffers(1, &ebo);


    glGenBuffers(SSBOS_COUNT, ssbos);

    for( unsigned int j = 0; j < SSBOS_COUNT; j++ ){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[j]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, j, ssbos[j]);
    }
    glGenBuffers(1,&buffer_utilitary);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    glGenTextures(1, &texUtilitary);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texUtilitary);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,0,0,0,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    setTexParameter(GL_TEXTURE_2D_ARRAY);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    for( int i = 0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        unsigned short k = pow(2,6+i);
        textureHandlers[i].texturesCount = 0;
        glGenTextures(1,&textureHandlers[i].texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY,textureHandlers[i].texture);
        glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,k,k,0,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);

        setTexParameter(GL_TEXTURE_2D_ARRAY);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Render_Data::free_data() {
    for( unsigned int i = 0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        free(textureHandlers[i].materialIndex);
        free(textureHandlers[i].emptyTextures);
    }


    compacted_meshes.free_data();

    // glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glDeleteBuffers(1, &buffer_utilitary);
    glDeleteBuffers(SSBOS_COUNT, ssbos);


    free(materials);
}
