#include "game.hpp"
#include "engine.hpp"
#include <GLFW/glfw3.h>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

void Game::compile() {
    engine.scripts_manager.compile();
}
void Game::runtime() {
    engine.scripts_manager.start();
    while(!glfwWindowShouldClose(engine.glfwWin)) {
        engine.scripts_manager.update();

        engine.newframe();

        engine.draw();

        engine.post_processing();

        engine.ui();


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(engine.glfwWin);
        glfwPollEvents();
        flags  = flags & ~(1&2);
    }
}
void Game::init() {

}

void Game::free_data() {

}
