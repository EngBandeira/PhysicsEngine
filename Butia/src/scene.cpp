
#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>

#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "scene.hpp"

#include "utils/common.hpp"
#include "butia.hpp"

void Scene::newframe() {
    glBindFramebuffer(GL_FRAMEBUFFER, butia->ante_fbo);

    glClearColor(1, 0, 0, .5);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, butia->post_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();




    // if( butia->render_data.delayCounter == 0 && flags & MODELS_CHANGE_FLAG ) update();

    if( butia->render_data.delayCounter > 0 ) butia->render_data.delayCounter--;

    flags = flags & ~(MODELS_CHANGE_FLAG);


    for(unsigned int i = 0; i < butia->draw_group_manager.groups_count; i++){
        butia->draw_group_manager.groups[i].update();
    }

}

void Scene::post_processing() {
    glUseProgram(butia->post_processing_program);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D,butia->post_tex);
    glUniform1i(glGetUniformLocation(butia->post_processing_program,"texToShowFrom"),10);
    glBindImageTexture(0, butia->tex_post_processed, 0, GL_TRUE, 0, GL_READ_WRITE,  GL_RGBA8 );
    // glGen
    glDispatchCompute(120,68,1);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
}


void Scene::draw() {

    for( unsigned short i = 0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D_ARRAY,butia->texture_manager.textureHandlers[i].texture);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, butia->ante_fbo);

    for( unsigned int i = 0; i < COMMON_SSBOS_COUNT; i++ ){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, butia->render_data.ssbos[i]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, COMMON_SSBOS[i], butia->render_data.ssbos[i]);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindVertexArray(butia->render_data.vao);
    glBindBuffer(GL_ARRAY_BUFFER, butia->render_data.vbo);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data.ebo);

#ifdef TRANSFORM_FEEDBACK_ENABLE
        glBindBuffer(GL_ARRAY_BUFFER, butia->TBO);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, butia->TBO);
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, butia->QUERY);
        glBeginTransformFeedback(GL_TRIANGLES);
#endif
    for(unsigned int i = 0; i < butia->draw_group_manager.groups_count; i++ ) {

        DrawGroup& draw_group = butia->draw_group_manager.groups[i];
        if(draw_group.objects_count == 0)
            continue;
        // Material& material = butia->material_manager.materials[draw_group.material];
        if(i == 1)
            glDepthMask(GL_FALSE);

        butia->shaders_manager.programs[draw_group.program].use();

        glUniformMatrix4fv(glGetUniformLocation(butia->shader_program.program, "view"), 1, GL_FALSE, glm::value_ptr(butia->camera.get_view()));
        glUniform1ui(glGetUniformLocation(butia->shader_program.program, "material_index"), draw_group.material);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_group.ebo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBOS::TextureVerticesIndexSSBO, draw_group.texture_vertices_index_ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBOS::MatricesSSBO, draw_group.matrices_ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBOS::VerticesOffsetSSBO, draw_group.vertices_offset_ssbo);


        glDrawElements(GL_TRIANGLES, draw_group.vertices_index_count, GL_UNSIGNED_INT,
                      (void*)0);
        glDepthMask(GL_TRUE);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#ifdef TRANSFORM_FEEDBACK_ENABLE
        glEndTransformFeedback();
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

        glFlush();

        GLuint primitives;
        glGetQueryObjectuiv(butia->QUERY, GL_QUERY_RESULT, &primitives);

        GLfloat *feedback = (GLfloat *)malloc(butia->feedbacksize);
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, butia->feedbacksize, feedback);

        printf("%u vecs written!\n\n", primitives);

        for( unsigned int i = 0; i < butia->feedbacknumber / 4; i++ )
            printf("[%d] = %f %f %f %f\n",
                    i,
                    feedback[4 * i],
                    feedback[4 * i + 1],
                    feedback[4 * i + 2],
                    feedback[4 * i + 3]
            );

        printf("\n");
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
#endif

    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, butia->ante_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, butia->post_fbo);
        glBlitFramebuffer(0, 0, 1920, 1080,
                          0, 0, 1920, 1080,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

}
