#include "utils/common.hpp"
#include "render_data.hpp"
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>

#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>

#include "utils/utils.hpp"



void Render_Data::Compacted_Meshes::init(){
    meshes = (Vadia_Mesh*)malloc(0);

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
    free(meshes);

    free(vertices);
    free(vertices_index);
    free(vertices_index_offsets);
}

void Render_Data::init() {
    compacted_meshes.init();


    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);


    glGenBuffers(COMMON_SSBOS_COUNT, ssbos);

    for( unsigned int j = 0; j < COMMON_SSBOS_COUNT; j++ ){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[j]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, COMMON_SSBOS[j], ssbos[j]);
    }
    glGenBuffers(1,&buffer_utilitary);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    glGenTextures(1, &tex_utilitary);
    glBindTexture(GL_TEXTURE_2D_ARRAY, tex_utilitary);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,0,0,0,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    utils.setTexParameter(GL_TEXTURE_2D_ARRAY);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Render_Data::free_data() {
    compacted_meshes.free_data();

    // glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glDeleteBuffers(1, &buffer_utilitary);
    glDeleteBuffers(COMMON_SSBOS_COUNT, ssbos);

}
