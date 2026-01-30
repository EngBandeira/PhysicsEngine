#include "common.hpp"
#include "game_object.hpp"
#include "material.hpp"
#include "render.hpp"
#include "vendor/glad/glad.h"

#include "draw_group.hpp"
#include <cstdlib>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>

unsigned int DrawGroup::addObject(unsigned int object) {
    if(render.objects_count <= object)
        logger.sendError("Object >= objects_count");

    GameObject& obj = render.objects[object];
    objects = (unsigned int*)realloc(objects, sizeof(int) * ++objects_count);
    objects[objects_count-1] = object;
    vertices_index_count += render.render_data.compacted_meshes.meshes[obj.mesh_index].index_count;
    texture_vertices_index_count += render.render_data.compacted_meshes.meshes[obj.mesh_index].index_count;
    matrices = (float*)realloc(matrices, 16 * sizeof(float) * objects_count);
    memcpy(matrices + 16 * (objects_count - 1),glm::value_ptr(obj.tranform.matrix),16 * sizeof(float));

    render.shaders_manager.programs[program].use();

    glUniform1ui(glGetUniformLocation(render.shaderProgram, "objects_count"), objects_count);


//----//----//----//----//----//----// EBO //----//----//----//----//----//----//----

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render.render_data.buffer_utilitary);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * vertices_index_count, 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_COPY_READ_BUFFER, ebo);
    glBindBuffer(GL_COPY_WRITE_BUFFER, render.render_data.buffer_utilitary);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
            sizeof(int) * (vertices_index_count - render.render_data.compacted_meshes.meshes[obj.mesh_index].index_count));


    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);


    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
            sizeof(int) * (vertices_index_count - render.render_data.compacted_meshes.meshes[obj.mesh_index].index_count),
            sizeof(int) * render.render_data.compacted_meshes.meshes[obj.mesh_index].index_count,
            render.render_data.compacted_meshes.vertices_index + render.render_data.compacted_meshes.vertices_index_offsets[obj.mesh_index]);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    unsigned int temp = render.render_data.buffer_utilitary;
    render.render_data.buffer_utilitary = ebo;
    ebo = temp;

//----//----//----//----//----//----//  MATRICES  //----//----//----//----//----//----

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, render.render_data.buffer_utilitary);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * sizeof(float) * objects_count, 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_COPY_READ_BUFFER, matrices_ssbo);
    glBindBuffer(GL_COPY_WRITE_BUFFER, render.render_data.buffer_utilitary);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
            16 * sizeof(float) * (objects_count - 1));


    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);


    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
             16 * sizeof(float) * (objects_count - 1),
             16 * sizeof(float),
             glm::value_ptr(obj.tranform.matrix));



    temp = render.render_data.buffer_utilitary;
    render.render_data.buffer_utilitary = matrices_ssbo;
    matrices_ssbo = temp;

//----//----//----//----//----//----// VERTICES OFFSET //----//----//----//----//----
    //alloc objects_count + 1
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, render.render_data.buffer_utilitary);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * (objects_count + 1), 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_COPY_READ_BUFFER, vertices_offset_ssbo);
    glBindBuffer(GL_COPY_WRITE_BUFFER, render.render_data.buffer_utilitary);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
            sizeof(int) * objects_count);


    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    unsigned int vertices_offset;

    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * (objects_count - 1), sizeof(int),&vertices_offset);

    vertices_offset += render.render_data.compacted_meshes.meshes[obj.mesh_index].vertices_count;

    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
             sizeof(int) * objects_count,
             sizeof(int), &vertices_offset);

    temp = render.render_data.buffer_utilitary;
    render.render_data.buffer_utilitary = vertices_offset_ssbo;
    vertices_offset_ssbo = temp;

//----//----//----//----//----//----// TEXTURE VERTICES INDEX //----//----//----//----//----

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, render.render_data.buffer_utilitary);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * texture_vertices_index_count, 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_COPY_READ_BUFFER, texture_vertices_index_ssbo);
    glBindBuffer(GL_COPY_WRITE_BUFFER, render.render_data.buffer_utilitary);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
            sizeof(int) * (texture_vertices_index_count - render.render_data.compacted_meshes.meshes[obj.mesh_index].index_count));


    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);


    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
            sizeof(int) * (texture_vertices_index_count - render.render_data.compacted_meshes.meshes[obj.mesh_index].index_count),
            sizeof(int) * render.render_data.compacted_meshes.meshes[obj.mesh_index].index_count,
            render.render_data.compacted_meshes.texture_vertices_index + render.render_data.compacted_meshes.texture_vertices_index_offsets[obj.mesh_index]);


    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

    temp = render.render_data.buffer_utilitary;
    render.render_data.buffer_utilitary = texture_vertices_index_ssbo;
    texture_vertices_index_ssbo = temp;

//----//----//----//----//----//----// TEXTURES VERTICES OFFSET //----//----//----//----//----//----

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, render.render_data.buffer_utilitary);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * objects_count, 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_COPY_READ_BUFFER, texture_vertices_offset_ssbo);
    glBindBuffer(GL_COPY_WRITE_BUFFER, render.render_data.buffer_utilitary);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
            sizeof(int) * (objects_count - 1));


    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    unsigned int texture_vertices_offset;

    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * (objects_count - 1), sizeof(int),&texture_vertices_offset);

    texture_vertices_offset += render.render_data.compacted_meshes.meshes[obj.mesh_index].texture_vertices_count;

    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
            sizeof(int) * (objects_count - 1),
            sizeof(int), &texture_vertices_offset);

    temp = render.render_data.buffer_utilitary;
    render.render_data.buffer_utilitary = texture_vertices_offset_ssbo;
    texture_vertices_offset_ssbo = temp;

//----//----//----//----//----//----//----//----//----//----//----//----//----//----

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, render.render_data.buffer_utilitary);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

    return objects_count - 1;
}

void DrawGroup::update() {
    for(unsigned int i = 0; i < objects_count; i++){
        memcpy(matrices + 16 * i, glm::value_ptr(render.objects[objects[i]].tranform.matrix), 16 * sizeof(float));
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, matrices_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * sizeof(float) * objects_count, matrices, GL_DYNAMIC_DRAW);
}

void DrawGroup::init() {

    glGenBuffers(1, &ebo);
    glGenBuffers(1, &matrices_ssbo);
    glGenBuffers(1, &vertices_offset_ssbo);
    glGenBuffers(1, &texture_vertices_index_ssbo);
    glGenBuffers(1, &texture_vertices_offset_ssbo);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertices_offset_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int), 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, texture_vertices_offset_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int), 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    objects_count = 0;
    objects = (unsigned int*)malloc(0);
    matrices = (float*)malloc(0);
    Material &mt = render.material_manager.materials[material];


    render.shaders_manager.programs[program].use();
    glUniform4f(glGetUniformLocation(render.shaderProgram, "material.K[0]"), mt.K[0], mt.K[1], mt.K[2], 1);
    glUniform4f(glGetUniformLocation(render.shaderProgram, "material.K[0]"), mt.K[3], mt.K[4], mt.K[5], 1);
    glUniform4f(glGetUniformLocation(render.shaderProgram, "material.K[0]"), mt.K[6], mt.K[7], mt.K[8], 1);

    glUniform1f(glGetUniformLocation(render.shaderProgram, "material.Ni"), mt.Ni);
    glUniform1f(glGetUniformLocation(render.shaderProgram, "material.d"), mt.d);
    glUniform1f(glGetUniformLocation(render.shaderProgram, "material.bm"), mt.bm);

    glUniform1ui(glGetUniformLocation(render.shaderProgram, "material.type"), mt.type);
    TextureLocation loc;
    if(mt.maps[0] != (unsigned int)-1){
        loc = render.texture_manager.textures[mt.maps[0]].location;
        glUniform1i(glGetUniformLocation(render.shaderProgram, "material.maps[0].handler"), loc.handler);
        glUniform1ui(glGetUniformLocation(render.shaderProgram, "material.maps[0].index"), loc.index);
    }
    if(mt.maps[1] != (unsigned int)-1){
        loc = render.texture_manager.textures[mt.maps[1]].location;
        glUniform1i(glGetUniformLocation(render.shaderProgram, "material.maps[1].handler"), loc.handler);
        glUniform1ui(glGetUniformLocation(render.shaderProgram, "material.maps[1].index"), loc.index);
    }
    if(mt.maps[2] != (unsigned int)-1){
        loc = render.texture_manager.textures[mt.maps[2]].location;
        glUniform1i(glGetUniformLocation(render.shaderProgram, "material.maps[2].handler"), loc.handler);
        glUniform1ui(glGetUniformLocation(render.shaderProgram, "material.maps[2].index"), loc.index);
    }
    if(mt.maps[3] != (unsigned int)-1){
        loc = render.texture_manager.textures[mt.maps[3]].location;
        glUniform1i(glGetUniformLocation(render.shaderProgram, "material.maps[3].handler"), loc.handler);
        glUniform1ui(glGetUniformLocation(render.shaderProgram, "material.maps[3].index"), loc.index);
    }

}

void DrawGroup::freeData() {
    free(matrices);
    free(objects);
    glDeleteBuffers(1,&matrices_ssbo);
    glDeleteBuffers(1,&vertices_offset_ssbo);
}
