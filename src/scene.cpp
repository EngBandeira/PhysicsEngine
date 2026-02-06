
#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>

#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "scene.hpp"
#include "engine.hpp"

void Scene::runtime() {
    while(!glfwWindowShouldClose(engine.glfwWin)) {
        engine.scripts_manager.update();

        newframe();

        draw();

        post_processing();

        ui();


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(engine.glfwWin);
        glfwPollEvents();
        flags  = flags & ~(1&2);
    }
}

void Scene::newframe() {
    glBindFramebuffer(GL_FRAMEBUFFER, engine.FBO_TO);

    glClearColor(1, 0, 0, .5);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, engine.FBO_FROM);
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

    if( engine.assets.framesDelay <= engine.assets.delayCounter ) {
        engine.assets.update();
        engine.assets.delayCounter = 0;
    }

    if( engine.render_data.delayCounter == 0 && flags & MODELS_CHANGE_FLAG ) update();

    if( engine.render_data.delayCounter > 0 ) engine.render_data.delayCounter--;

    flags = flags & ~(MODELS_CHANGE_FLAG);

    engine.assets.delayCounter++;

    for(unsigned int i = 0; i < engine.draw_group_manager.groups_count; i++){
        engine.draw_group_manager.groups[i].update();
    }

}

void Scene::post_processing() {
    glUseProgram(engine.post_program);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D,engine.texToShowFrom);
    glUniform1i(glGetUniformLocation(engine.post_program,"texToShowFrom"),10);
    glBindImageTexture(0, engine.tex_post, 0, GL_TRUE, 0, GL_READ_WRITE,  GL_RGBA8 );
    // glGen
    glDispatchCompute(120,68,1);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
}


void Scene::draw() {

    for( unsigned short i = 0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D_ARRAY,engine.texture_manager.textureHandlers[i].texture);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, engine.FBO_FROM);

    for( unsigned int i = 0; i < COMMON_SSBOS_COUNT; i++ ){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, engine.render_data.ssbos[i]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, COMMON_SSBOS[i], engine.render_data.ssbos[i]);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindVertexArray(engine.render_data.vao);
    glBindBuffer(GL_ARRAY_BUFFER, engine.render_data.vbo);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data.ebo);


    if(engine.transFeed) {
        glBindBuffer(GL_ARRAY_BUFFER, engine.TBO);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, engine.TBO);
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, engine.QUERY);
        glBeginTransformFeedback(GL_TRIANGLES);
    }

    for(unsigned int i = 0; i < engine.draw_group_manager.groups_count; i++ ) {

        DrawGroup& draw_group = engine.draw_group_manager.groups[i];
        // Material& material = engine.material_manager.materials[draw_group.material];
        if(i == 1)
            glDepthMask(GL_FALSE);

        engine.shaders_manager.programs[draw_group.program].use();

        glUniformMatrix4fv(glGetUniformLocation(engine.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(engine.camera.get_view()));
        glUniform1ui(glGetUniformLocation(engine.shaderProgram, "material_index"), draw_group.material);

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

    if(engine.transFeed) {
        glEndTransformFeedback();
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

        glFlush();

        GLuint primitives;
        glGetQueryObjectuiv(engine.QUERY, GL_QUERY_RESULT, &primitives);

        GLfloat *feedback = (GLfloat *)malloc(engine.feedbacksize);
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, engine.feedbacksize, feedback);

        printf("%u vecs written!\n\n", primitives);

        for( unsigned int i = 0; i < engine.feedbacknumber / 4; i++ )
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
        glBindFramebuffer(GL_READ_FRAMEBUFFER, engine.FBO_FROM);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, engine.FBO_TO);
        glBlitFramebuffer(0, 0, 1920, 1080,
                          0, 0, 1920, 1080,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

}
