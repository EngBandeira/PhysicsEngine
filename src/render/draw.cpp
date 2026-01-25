#include "draw_group.hpp"
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

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data.ebo);


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

    for( int i = 0; i < render_data.compacted_meshes.draw_groups_count; i++ ) {
        //pinto
        // if(render_data.compacted_meshes.mesh_objects[i] == (unsigned int )-1)
        //     continue;
        DrawGroup& draw_group = render_data.compacted_meshes.draw_groups[i];
        // GameObject& obj = objects[draw_group.objects[i]];
        // glUniform1ui(glGetUniformLocation(shaderProgram, "texture_vertices_offset"),render_data.compacted_meshes.texture_vertices_offsets[i]);
        // glUniform1ui(glGetUniformLocation(shaderProgram, "texture_vertices_index_offset"),render_data.compacted_meshes.texture_vertices_index_offsets[i]);

        glUniform1i(glGetUniformLocation(shaderProgram, "material.maps[0].handler"), draw_group.material.maps[0].handler);
        glUniform1ui(glGetUniformLocation(shaderProgram, "material.maps[0].index"), draw_group.material.maps[0].index);
        glUniform1ui(glGetUniformLocation(shaderProgram, "objects_count"), draw_group.objects_count);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_group.ebo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBOS::MatricesSSBO, draw_group.matrices_ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBOS::VerticesOffsetSSBO, draw_group.vertices_offset_ssbo);


        glDrawElements(GL_TRIANGLES, draw_group.vertices_index_count, GL_UNSIGNED_INT,
                      (void*)0);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
