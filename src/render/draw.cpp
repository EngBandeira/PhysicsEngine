#include "game_object.hpp"
#include "render.hpp"

#include <X11/X.h>
#include <glm/common.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/trigonometric.hpp>
#include <cstdio>
#include <glm/geometric.hpp>
#include <glm/exponential.hpp>
#include <unistd.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <stdlib.h>
#include "common.hpp"

#include "render_data.hpp"
#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>

void Render::draw() {

    for( unsigned short i = 0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D_ARRAY,render_data.textureHandlers[i].texture);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_FROM);

    for( unsigned int i = 0; i < SSBOS_COUNT; i++ ){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, render_data.ssbos[i]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, render_data.ssbos[i]);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindVertexArray(render_data.vao);
    glBindBuffer(GL_ARRAY_BUFFER, render_data.vbo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data.ebo);


    if(transFeed) {
        glBindBuffer(GL_ARRAY_BUFFER, TBO);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, TBO);
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, QUERY);
        glBeginTransformFeedback(GL_TRIANGLES);
    }

    // struct Material {
    //     float K[9];
    //     float Ni, d, bm;
    //     TextureLocation maps[4];
    //     uint type;
    // };

    // uniform Material material;

    // struct TextureLocation {
    //     int handler;
    //     uint index;
    // };

    for( int i = 0; i < render_data.compacted_meshes.meshes_count; i++ ) {
        //pinto
        if(render_data.compacted_meshes.mesh_objects[i] == (unsigned int )-1)
            continue;
        GameObject& obj = objects[render_data.compacted_meshes.mesh_objects[i]];
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model_matrix"), 1, GL_FALSE, glm::value_ptr(obj.tranform.matrix));
        glUniform1ui(glGetUniformLocation(shaderProgram, "texture_vertices_offset"),render_data.compacted_meshes.texture_vertices_offsets[i]);
        glUniform1ui(glGetUniformLocation(shaderProgram, "texture_vertices_index_offset"),render_data.compacted_meshes.texture_vertices_index_offsets[i]);

        glUniform1i(glGetUniformLocation(shaderProgram, "material.maps[0].handler"),obj.material.maps[0].handler);
        glUniform1ui(glGetUniformLocation(shaderProgram, "material.maps[0].index"),obj.material.maps[0].index);


        glBindVertexArray(render_data.vao);
        glBindBuffer(GL_ARRAY_BUFFER,render_data.vbo);
        glBufferData(GL_ARRAY_BUFFER,  sizeof(float) * render_data.compacted_meshes.meshes[i].verticesCount,
                            render_data.compacted_meshes.vertices+render_data.compacted_meshes.vertices_offsets[i],GL_DYNAMIC_DRAW);

        glDrawElements(GL_TRIANGLES, render_data.compacted_meshes.meshes[i].verticesIndexCount, GL_UNSIGNED_INT,
                      (void*)(render_data.compacted_meshes.vertices_index_offsets[i]*sizeof(int)));

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }

    if(transFeed) {
        glEndTransformFeedback();
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

        glFlush();

        GLuint primitives;
        glGetQueryObjectuiv(QUERY, GL_QUERY_RESULT, &primitives);

        GLfloat *feedback = (GLfloat *)malloc(feedbacksize);
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbacksize, feedback);

        printf("%u vecs written!\n\n", primitives);

        for( unsigned int i = 0; i < feedbacknumber / 4; i++ )
            printf("[%d] = %f %f %f %f\n",
                    i,
                    feedback[4 * i],
                    feedback[4 * i + 1],
                    feedback[4 * i + 2],
                    feedback[4 * i + 3]
            );

        printf("\n");
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO_FROM);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_TO);
        glBlitFramebuffer(0, 0, 1920, 1080,
                          0, 0, 1920, 1080,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

}
