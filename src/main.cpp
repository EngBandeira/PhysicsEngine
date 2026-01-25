#include "common.hpp"

#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <bits/stdc++.h>
#include <stdio.h>

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

// 4.6 (Core Profile) Mesa 25.1.3-arch1.3
int main() {
    init();

    render = Render(glfw_window);

    logger.terminal.lines.push_back((char*)"Console Init\n");
    // render.render_data.addMesh(MeshGenData{
    //     .path = (char*)"assets/3dmodels/Cube.obj"
    // });
    // render.addObject(GameObjectGenData{
    //     .name = "Cube",
    //     .uuid = "a"
    // });
    // render.objects[0].material.maps[0] = render.render_data.addTexToHandler("assets/3dmodels/Cube.jpg");
    // render.render_data.compacted_meshes.mesh_objects[0] = 0;

    // render.render_data.addMesh(MeshGenData{
    //     .path = (char*)"assets/3dmodels/Cube.obj"
    // });
    // render.addObject(GameObjectGenData{
    //     .name = "Piston",
    //     .uuid = "a"
    // });
    // render.objects[1].material.maps[0] = render.render_data.addTexToHandler("assets/3dmodels/Piston.jpg");
    // render.render_data.compacted_meshes.mesh_objects[1] = 1;

    unsigned int draw_groups[2];
    draw_groups[0] = render.render_data.alloc_draw_group();
    draw_groups[1] = render.render_data.alloc_draw_group();

    render.render_data.compacted_meshes.draw_groups[draw_groups[0]].init(render.shader);
    render.render_data.compacted_meshes.draw_groups[draw_groups[0]].material.maps[0] =
                render.render_data.add_tex_to_handler((char*)"assets/3dmodels/cannon_01_diff_4k.jpg");
    render.render_data.compacted_meshes.draw_groups[draw_groups[0]].addObject(
        render.addObject(GameObjectGenData {
            .name = (char*)"Canhao",
            .uuid = (char*)"a",
            .mesh_index = render.render_data.add_mesh(MeshGenData {
                .path = (char*)"assets/3dmodels/cannon_01_4k.obj"
            })
        })
    );

    render.render_data.compacted_meshes.draw_groups[draw_groups[1]].init(render.shader);
    render.render_data.compacted_meshes.draw_groups[draw_groups[1]].material.maps[0] =
                render.render_data.add_tex_to_handler((char*)"assets/3dmodels/marble_bust_01_diff_4k.jpg");
    render.render_data.compacted_meshes.draw_groups[draw_groups[1]].addObject(
        render.addObject(GameObjectGenData {
            .name = (char*)"Busto",
            .uuid = (char*)"a",
            .mesh_index = render.render_data.add_mesh(MeshGenData {
                .path = (char*)"assets/3dmodels/marble_bust_01_4k.obj"
            })
        })
    );


    while(!glfwWindowShouldClose(glfw_window)) {
        render.newframe();

        render.draw();

        render.ui();


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glfw_window);
        glfwPollEvents();
        flags  = flags & ~(1&2);
    }

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
