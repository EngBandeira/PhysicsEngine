
#include <X11/X.h>
#include <glm/common.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/exponential.hpp>
#include <unistd.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <stdlib.h>

#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>

#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "render.hpp"

void Render::newframe() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_TO);

    glClearColor(0.7f, 0.3f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO_FROM);
    glClearColor(0.07f, 0.13f, 0.27f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();


    // glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera.viewMatrix));

    // glm::vec4 po = glm::inverse(camera.get_view_matrix())*glm::vec4(0, 0, 0, 1);
    // glm::vec4 po = glm::vec4(camera.get_position(),1);
    // glUniform4f(glGetUniformLocation(shaderProgram,"camPosition"),po.x, po.y, po.z, po.w);
    // glUniform1i(glGetUniformLocation(shaderProgram,"normalATIVO"),normalATIVO);
    // glUniform1f(glGetUniformLocation(shaderProgram,"normalV"),normalV);

    if( assets.framesDelay <= assets.delayCounter ) {
        assets.update();
        assets.delayCounter = 0;
    }

    if( render_data.delayCounter == 0 && flags & MODELS_CHANGE_FLAG ) update();

    if( render_data.delayCounter > 0 ) render_data.delayCounter--;

    flags = flags & ~(MODELS_CHANGE_FLAG);

    assets.delayCounter++;

    for(unsigned int i = 0; i < render.draw_group_manager.groups_count; i++){
        render.draw_group_manager.groups[i].update();
    }

}
