
#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>

#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "scene.hpp"
#include "render.hpp"

void Scene::runtime() {
    while(!glfwWindowShouldClose(render.glfwWin)) {
        render.scripts_manager.update();

        newframe();

        draw();

        post_processing();

        ui();


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(render.glfwWin);
        glfwPollEvents();
        flags  = flags & ~(1&2);
    }
}

void Scene::newframe() {
    glBindFramebuffer(GL_FRAMEBUFFER, render.FBO_TO);

    glClearColor(1, 0, 0, .5);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, render.FBO_FROM);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();


    // glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera.viewMatrix));

    // glm::vec4 po = glm::inverse(camera.get_view_matrix())*glm::vec4(0, 0, 0, 1);
    // glm::vec4 po = glm::vec4(camera.get_position(),1);
    // glUniform4f(glGetUniformLocation(shaderProgram,"camPosition"),po.x, po.y, po.z, po.w);
    // glUniform1i(glGetUniformLocation(shaderProgram,"normalATIVO"),normalATIVO);
    // glUniform1f(glGetUniformLocation(shaderProgram,"normalV"),normalV);

    if( render.assets.framesDelay <= render.assets.delayCounter ) {
        render.assets.update();
        render.assets.delayCounter = 0;
    }

    if( render.render_data.delayCounter == 0 && flags & MODELS_CHANGE_FLAG ) update();

    if( render.render_data.delayCounter > 0 ) render.render_data.delayCounter--;

    flags = flags & ~(MODELS_CHANGE_FLAG);

    render.assets.delayCounter++;

    for(unsigned int i = 0; i < render.draw_group_manager.groups_count; i++){
        render.draw_group_manager.groups[i].update();
    }

}

void Scene::post_processing() {
    glUseProgram(render.post_program);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D,render.texToShowFrom);
    glUniform1i(glGetUniformLocation(render.post_program,"texToShowFrom"),10);
    glBindImageTexture(0, render.tex_post, 0, GL_TRUE, 0, GL_READ_WRITE,  GL_RGBA8 );
    // glGen
    glDispatchCompute(120,68,1);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
}


void Scene::draw() {

    for( unsigned short i = 0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D_ARRAY,render.texture_manager.textureHandlers[i].texture);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, render.FBO_FROM);

    for( unsigned int i = 0; i < COMMON_SSBOS_COUNT; i++ ){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, render.render_data.ssbos[i]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, COMMON_SSBOS[i], render.render_data.ssbos[i]);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindVertexArray(render.render_data.vao);
    glBindBuffer(GL_ARRAY_BUFFER, render.render_data.vbo);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data.ebo);


    if(render.transFeed) {
        glBindBuffer(GL_ARRAY_BUFFER, render.TBO);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, render.TBO);
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, render.QUERY);
        glBeginTransformFeedback(GL_TRIANGLES);
    }

    for(unsigned int i = 0; i < render.draw_group_manager.groups_count; i++ ) {

        DrawGroup& draw_group = render.draw_group_manager.groups[i];
        // Material& material = render.material_manager.materials[draw_group.material];
        if(i == 1)
            glDepthMask(GL_FALSE);

        render.shaders_manager.programs[draw_group.program].use();

        glUniformMatrix4fv(glGetUniformLocation(render.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(render.++++++++camera.get_view()));
        glUniform1ui(glGetUniformLocation(render.shaderProgram, "material_index"), draw_group.material);

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

    if(render.transFeed) {
        glEndTransformFeedback();
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

        glFlush();

        GLuint primitives;
        glGetQueryObjectuiv(render.QUERY, GL_QUERY_RESULT, &primitives);

        GLfloat *feedback = (GLfloat *)malloc(render.feedbacksize);
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, render.feedbacksize, feedback);

        printf("%u vecs written!\n\n", primitives);

        for( unsigned int i = 0; i < render.feedbacknumber / 4; i++ )
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
        glBindFramebuffer(GL_READ_FRAMEBUFFER, render.FBO_FROM);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, render.FBO_TO);
        glBlitFramebuffer(0, 0, 1920, 1080,
                          0, 0, 1920, 1080,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

}
