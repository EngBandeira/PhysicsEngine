#include "common.hpp"

#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <bits/stdc++.h>
#include <stdio.h>

#include "material.hpp"
#include "mesh.hpp"
#include "render_data.hpp"
#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "render.hpp"
GLFWwindow *glfw_window;
void init();

#include "utils.hpp"
// watch set variable render.draw_group_manager.groups[2].objects_count
// 4.6 (Core Profile) Mesa 25.1.3-arch1.3
int main() {
    init();

    render = Render(glfw_window);

    render.scripts_manager.init();
    render.scripts_manager.get_script("/src/caralha/pinto.rs");

    render.scripts_manager.compile();

    logger.terminal.lines.push_back((char*)"Console Init\n");
    render.draw_group_manager.get_draw_group (
           Gen_Shader {
               .vertex = (char*)"shader.vert",
               .geometric = (char*)"shader.geom",
               .fragment = (char*)"color.frag"
               },MaterialGenData());

    render.draw_group_manager.get_draw_group (
           Gen_Shader {
               .vertex = (char*)"shader.vert",
               .geometric = (char*)"shader.geom",
               .fragment = (char*)"grid.frag"
               },
           MaterialGenData());

    render.draw_group_manager.get_draw_group (
           Gen_Shader {
               .vertex = (char*)"shader.vert",
               .geometric = (char*)"shader.geom",
               .fragment = (char*)"texture.frag"
               },
           MaterialGenData((char*)"assets/3dmodels/Seta.png"));

    render.draw_group_manager.get_draw_group (
           Gen_Shader {
               .vertex = (char*)"shader.vert",
               .geometric = (char*)"shader.geom",
               .fragment = (char*)"texture.frag"
               },
           MaterialGenData((char*)"assets/3dmodels/Cubemap.jpg"));


    render.default_game_objects.grid = render.addObject("assets/3dmodels/Plane.obj","Grid", 1);

    render.default_game_objects.seta = render.addObject("assets/3dmodels/Seta.obj","Seta", 2);

    render.addObject("assets/3dmodels/Cubemap.obj","Cubemap", 3);




    // render.draw_group_manager.get_draw_group (
    //        Gen_Shader {
    //            .vertex = (char*)VERTEX_SHADERS_LOCALPATH,
    //            .geometric = (char*)GEOMETRY_SHADERS_LOCALPATH,
    //            .fragment = (char*)FRAGMENT_SHADERS_LOCALPATH
    //            },
    //        MaterialGenData((char*)"assets/3dmodels/marble_bust_01_diff_4k.jpg")
    // );

    // render.draw_group_manager.get_draw_group(
    //     Gen_Shader {
    //         .vertex = (char*)VERTEX_SHADERS_LOCALPATH,
    //         .geometric = (char*)GEOMETRY_SHADERS_LOCALPATH,
    //         .fragment = (char*)"fShader1.frag"
    //         },
    //     MaterialGenData((char*)"assets/3dmodels/cannon_01_diff_4k.jpg")
    // );


    // render.addObject("Busto","assets/3dmodels/marble_bust_01_4k.obj", 0);

    // render.addObject("Canhao","assets/3dmodels/cannon_01_4k.obj", 1);


    while(!glfwWindowShouldClose(glfw_window)) {
        render.newframe();

        render.draw();

        render.post_processing();

        render.ui();


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glfw_window);
        glfwPollEvents();
        flags  = flags & ~(1&2);
    }
    render.free_data();
    return 0;
}

void error_callback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    if( type == GL_DEBUG_TYPE_ERROR ) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = "
                "0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                type,
                severity,
                message
        );
    }
}


void init(){
    setenv("XDG_SESSION_TYPE", "x11" , 1); // THIS IN NECESSARY TO USE THE RENDERDOC, IT DONT RUNS ON WAYLAND
    glfwInit();
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    glfw_window = glfwCreateWindow(SCR_X, SCR_Y, "PhysicsEngine", NULL, NULL);
    if( glfw_window == NULL ) {
        glfwTerminate();
        logger.sendError("failed to crate a Glfw Window", 1);
    }

    glfwMakeContextCurrent(glfw_window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(glfw_window,
        [](GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
        });

    if(! gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) ) {
        logger.sendError("cannot init glad", 1);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(error_callback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    glfwMakeContextCurrent(glfw_window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    glfwSetKeyCallback(glfw_window, ImGui_ImplGlfw_KeyCallback);
}

void close(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}
