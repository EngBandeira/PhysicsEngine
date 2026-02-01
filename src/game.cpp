#include "game.hpp"
#include "render.hpp"
#include <GLFW/glfw3.h>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

void Game::compile() {
    render.scripts_manager.compile();
}
void Game::runtime() {
    render.scripts_manager.start();
    while(!glfwWindowShouldClose(render.glfwWin)) {
        render.scripts_manager.update();

        render.newframe();

        render.draw();

        render.post_processing();

        render.ui();


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(render.glfwWin);
        glfwPollEvents();
        flags  = flags & ~(1&2);
    }
}
void Game::init() {

}

void Game::free_data() {

}
