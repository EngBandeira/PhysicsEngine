#include "render.hpp"
#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <bits/stdc++.h>
#include <stdio.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "stb_image/stb_image.h"

#include "model.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "vertexArray.hpp"

// #define PI 3.14
#define VERTEX_SHADERS_LOCALPATH "vShader.vert"
#define FRAGMENT_SHADERS_LOCALPATH "fShader.frag"
#define GEOMETRY_SHADERS_LOCALPATH "gShader.geom"

// 4.6 (Core Profile) Mesa 25.1.3-arch1.3
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, int key, int scancode, int action,
                  int mods);
void error_callback(GLenum source, GLenum type, unsigned int id,
                    GLenum severity, GLsizei length, const GLchar *message,
                    const void *userParam);

// settings
#define SCR_X 1920
#define SCR_Y 1080

// float *vertices;
glm::mat4 projMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelMatrix;
void pr(){
    // printf("bah\n");
}

int main()
{
    setenv("XDG_SESSION_TYPE", "x11" , 1); // THIS IN NECESSARY TO USE THE RENDERDOC, IT DONT RUNS ON WAYLAND
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *bigWindow =
        glfwCreateWindow(SCR_X, SCR_Y, "PhysicsEngine", NULL, NULL);

    if (bigWindow == NULL)
    {
        sendError("failed to crate a Glfw Window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(bigWindow);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(bigWindow,
                                    [](GLFWwindow *window, int width, int height) {
                                    glViewport(0, 0, width, height);
                                    });
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        sendError("cannot init glad");
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(&error_callback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                            GL_TRUE);
    glfwMakeContextCurrent(bigWindow);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle &style = ImGui::GetStyle();

    ImGui_ImplGlfw_InitForOpenGL(bigWindow, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    glfwSetKeyCallback(bigWindow,ImGui_ImplGlfw_KeyCallback);

    projMatrix = glm::perspective(glm::radians(45.0f),
                                    (float)SCR_X / (float)SCR_Y, 0.1f, 100.0f);
        viewMatrix = glm::mat4(1);
        viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -5.0f));


    {
        Render m_render(viewMatrix,projMatrix,bigWindow);



        // Model model2("assets/3dmodels/cannon_01_4k.obj","assets/3dmodels/cannon_01_diff_4k.jpg");

        // Model model1("assets/3dmodels/SphereLow.obj","assets/3dmodels/CubeTexture2.jpg");
        // Model model1("assets/3dmodels/Cube.obj","assets/3dmodels/CubeTexture2.jpg");

        // model2.mesh.matrix = glm::translate(model2.mesh.matrix, glm::vec3(1.0f, 0.0f,0.0f));
        // model1.mesh.matrix = glm::translate(model1.mesh.matrix, glm::vec3(0.0f, 0.0f, -5.0f));
        ModelGenStruct models[2] =
        {ModelGenStruct
        {
            meshPath:"assets/3dmodels/cannon_01_4k.obj",
            texPath:"assets/3dmodels/cannon_01_diff_4k.jpg"
        },
            ModelGenStruct
            {
                meshPath:"assets/3dmodels/Cube.obj",
                texPath:"assets/3dmodels/CubeTexture2.jpg"
            }

        };


        ;
        // m_render.addModels(1, models);
        unsigned short i = 1;
        // m_render.rmModels(1, &i);

        int a =1;
        // m_render.addModels(1, model);
        // ;
        // m_render.addModels(1, model);
        // unsigned short i = 0;
        // m_render.rmModels(1, &i);



        m_render.start(pr,pr,pr);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(bigWindow);
    glfwTerminate();

    return 0;
}
void error_callback(GLenum source, GLenum type, unsigned int id,
                    GLenum severity, GLsizei length, const GLchar *message,
                    const void *userParam)
{
    if (type == GL_DEBUG_TYPE_ERROR)
    {
        fprintf(stderr,
                "GL CALLBACK: %s type = 0x%x, severity = "
                "0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
                severity, message);
    }
}



void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
// GL CALLBACK:  type = 0x8251, severity = 0x826b, message = Shader Stats:
// SGPRS: 16 VGPRS: 8 Code Size: 52 LDS: 0 Scratch: 0 Max Waves: 20 Spilled
// SGPRs: 0 Spilled VGPRs: 0 PrivMem VGPRs: 0 LSOutputs: 0 HSOutputs: 0
// HSPatchOuts: 0 ESOutputs: 0 GSOutputs: 0 VSOutputs: 0 PSOutputs: 1
// InlineUniforms: 0 DivergentLoop: 0 (PS, W32)
