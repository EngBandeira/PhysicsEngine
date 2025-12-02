#define STB_IMAGE_IMPLEMENTATION
#include <bits/stdc++.h>
#include <iostream>
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
void framebuffer_size_callback (GLFWwindow *window, int width, int height);
void processInput (GLFWwindow *window, int key, int scancode, int action,
                   int mods);
void error_callback (GLenum source, GLenum type, unsigned int id,
                     GLenum severity, GLsizei length, const GLchar *message,
                     const void *userParam);

// settings
const unsigned int SCR_X = 800;
const unsigned int SCR_Y = 600;

float *vertices;
glm::mat4 projMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelMatrix;

int
main ()
{
        Mesh cube ("assets/3dmodels/Cube.obj");

        glfwInit ();
        glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        GLFWwindow *bigWindow
            = glfwCreateWindow (SCR_X, SCR_Y, "PhysicsEngine", NULL, NULL);

        if (bigWindow == NULL)
                {
                        sendError ("failed to crate a Glfw Window");
                        glfwTerminate ();
                        return -1;
                }
        glfwMakeContextCurrent (bigWindow);
        glfwSwapInterval (1);
        glfwSetFramebufferSizeCallback (
            bigWindow, [] (GLFWwindow *window, int width, int height)
                    { glViewport (0, 0, width, height); });

        if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress))
                {
                        sendError ("cannot init glad");
                        return -1;
                }
        glEnable (GL_DEPTH_TEST);
        glEnable (GL_DEBUG_OUTPUT);
        glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDebugMessageCallback (&error_callback, 0);
        glDebugMessageControl (GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                               nullptr, GL_TRUE);

        IMGUI_CHECKVERSION ();
        ImGui::CreateContext ();
        ImGuiIO &io = ImGui::GetIO ();
        (void)io;
        io.ConfigFlags
            |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags
            |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark ();
        // ImGui::StyleColorsLight();

        // Setup scaling
        ImGuiStyle &style = ImGui::GetStyle ();
        // style.ScaleAllSizes(main_scale);        // Bake a fixed style scale.
        // (until we have a solution for dynamic style scaling, changing this
        // requires resetting Style + calling this again) style.FontScaleDpi =
        // main_scale;        // Set i
        ImGui_ImplGlfw_InitForOpenGL (bigWindow, true);
        // ImGui_ImplOpenGL3_Inst("#version 460 core");
        ImGui_ImplOpenGL3_Init ("#version 460");

        Texture texture ("assets/3dmodels/CubeTexture2.jpg");

        Shader shader ((const char *[3]){ VERTEX_SHADERS_LOCALPATH,
                                          FRAGMENT_SHADERS_LOCALPATH,
                                          GEOMETRY_SHADERS_LOCALPATH });

        unsigned int shaderProgram = glCreateProgram ();
        shader.attach (shaderProgram);

        // const GLchar *feedbackVaryings[] = { "outValue" };
        // glTransformFeedbackVaryings (shaderProgram, 1, feedbackVaryings,
        //                              GL_INTERLEAVED_ATTRIBS);

        glLinkProgram (shaderProgram);
        // shader.~Shader();
        glUseProgram (shaderProgram);

        // note that we're translating the scene in the reverse direction of
        // where we want to move
        projMatrix = glm::perspective (
            glm::radians (45.0f), (float)SCR_X / (float)SCR_Y, 0.1f, 100.0f);
        viewMatrix = glm::mat4 (1);
        modelMatrix = glm::mat4 (1);
        viewMatrix
            = glm::translate (viewMatrix, glm::vec3 (0.0f, 0.0f, -5.0f));

        VAO m_VAO = VAO ();

        unsigned int EBO, TBO, Query, texSSBO, texIndexSSBO;
        glGenBuffers (1, &EBO);
        glGenQueries (1, &Query);
        glGenBuffers (1, &texSSBO);
        glGenBuffers (1, &texIndexSSBO);

        m_VAO.bind ();
        m_VAO.createVBO ();
        m_VAO.bindVBO (0);

        //     glBindBuffer (GL_ARRAY_BUFFER, VBO);
        glBufferData (GL_ARRAY_BUFFER, sizeof (float) * cube.verticesCount,
                      vertices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray (0);
        glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float),
                               (void *)0); // set vec4 position

        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                      sizeof (unsigned int) * cube.verticesIndexCount,
                      cube.verticesIndex, GL_STATIC_DRAW);

        glBindBuffer (GL_SHADER_STORAGE_BUFFER, texSSBO);
        glBufferData (GL_SHADER_STORAGE_BUFFER,
                      sizeof (float) * cube.textureVerticesCount,
                      cube.textureVertices, GL_DYNAMIC_DRAW);
        glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 2, texSSBO);
        glBindBuffer (GL_SHADER_STORAGE_BUFFER, 0); // unbind

        glBindBuffer (GL_SHADER_STORAGE_BUFFER, texIndexSSBO);
        glBufferData (GL_SHADER_STORAGE_BUFFER,
                      sizeof (unsigned int) * cube.textureIndexCount,
                      cube.textureIndex, GL_DYNAMIC_DRAW);
        glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 3, texIndexSSBO);
        glBindBuffer (GL_SHADER_STORAGE_BUFFER, 0); // unbind

        // glGenBuffers (1, &TBO);
        // glBindBuffer (GL_ARRAY_BUFFER, TBO);
        // glBufferData (GL_ARRAY_BUFFER, sizeof (float) * indexCount * 4 /3 ,
        // nullptr,
        //               GL_STATIC_READ);

        glBindBuffer (GL_ARRAY_BUFFER, 0);
        m_VAO.unbind ();

        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        glUniformMatrix4fv (glGetUniformLocation (shaderProgram, "projection"),
                            1, GL_FALSE, glm::value_ptr (projMatrix));
        texture.Bind (0);

        glUniform1i (glGetUniformLocation (shaderProgram, "Texture"), 0);
        glfwSetKeyCallback (bigWindow, processInput);

        unsigned int fbo;

        glGenFramebuffers (1, &fbo);
        glBindFramebuffer (GL_FRAMEBUFFER, fbo);

        unsigned int texture1;
        glGenTextures (1, &texture1);
        glBindTexture (GL_TEXTURE_2D, texture1);

        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, SCR_X / 2, SCR_Y / 2, 0,
                      GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_2D, texture1, 0);

        unsigned int rbo;
        glGenRenderbuffers (1, &rbo);
        glBindRenderbuffer (GL_RENDERBUFFER, rbo);
        glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_X / 2,
                               SCR_Y / 2);
        glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_RENDERBUFFER, rbo);
        glBindFramebuffer (GL_FRAMEBUFFER, 0);
        glBindTexture (GL_TEXTURE_2D, 0);
        glBindRenderbuffer (GL_RENDERBUFFER, 0);

        glBindFramebuffer (GL_FRAMEBUFFER, 0);

        while (!glfwWindowShouldClose (bigWindow))
                {
                        ImGui_ImplGlfw_NewFrame ();
                        ImGui_ImplOpenGL3_NewFrame ();

                        glClearColor (0.07f, 0.13f, 0.17f, 1.0f);
                        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                        ImGui::NewFrame ();
                        ImGui::Begin ("PintoGrande");

                        ImGui::GetWindowDrawList ()->AddImage (
                            (void *)texture1, ImVec2 (SCR_X / 2, SCR_Y / 2),
                            ImVec2 (SCR_X, SCR_Y), ImVec2 (0, 1),
                            ImVec2 (1, 0));

                        ImGui::End ();
                        bool a = 1;
                        ImGui::ShowDemoWindow (&a);
                        glUseProgram (shaderProgram);

                        glUniformMatrix4fv (
                            glGetUniformLocation (shaderProgram, "view"), 1,
                            GL_FALSE, glm::value_ptr (viewMatrix));
                        glUniformMatrix4fv (
                            glGetUniformLocation (shaderProgram, "model"), 1,
                            GL_FALSE, glm::value_ptr (modelMatrix));

                        // glBindBufferBase (GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                        // TBO);

                        // Used to update the vbo but without change the n of
                        // Vertex
                        m_VAO.bindVBO (0);
                        glBufferSubData (GL_ARRAY_BUFFER, 0,
                                         sizeof (float) * cube.verticesCount,
                                         vertices);

                        ImGui::Render ();
                        ImGui_ImplOpenGL3_RenderDrawData (
                            ImGui::GetDrawData ());

                        m_VAO.bind ();
                        glBindFramebuffer (GL_FRAMEBUFFER, fbo);

                        glDrawElements (GL_TRIANGLES, cube.verticesIndexCount,
                                        GL_UNSIGNED_INT, 0);
                        glBindFramebuffer (GL_FRAMEBUFFER, 0);

                        // glBeginQuery
                        // (GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
                        // Query);
                        // glBeginTransformFeedback (GL_TRIANGLES);

                        // glEndTransformFeedback ();
                        // glEndQuery
                        // (GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

                        // glFlush ();

                        // GLuint primitives;
                        // glGetQueryObjectuiv (Query, GL_QUERY_RESULT,
                        // &primitives);

                        // GLfloat feedback[4 * indexCount/3];
                        // glGetBufferSubData (GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                        //                     sizeof (feedback), feedback);

                        // printf ("%u primitives written!\n\n", primitives);

                        // for (int i = 0; i < indexCount/3; i++)
                        //     {
                        //         printf ("{%f , %f , %f , %f}\n", feedback[4
                        //         * i],
                        //                 feedback[4 * i + 1], feedback[4 * i
                        //                 + 2], feedback[4 * i + 3]);
                        //     }

                        glfwSwapBuffers (bigWindow);
                        glfwPollEvents ();
                        // sleep (1);
                }
        ImGui_ImplOpenGL3_Shutdown ();
        ImGui_ImplGlfw_Shutdown ();
        ImGui::DestroyContext ();

        glDeleteQueries (1, &Query);
        glDeleteVertexArrays (1, &m_VAO.name);
        glDeleteBuffers (1, &m_VAO.vbos[0].name);
        glDeleteBuffers (1, &EBO);
        glDeleteBuffers (1, &texSSBO);
        glDeleteBuffers (1, &texIndexSSBO);
        glDeleteProgram (shaderProgram);
        glfwDestroyWindow (bigWindow);
        glfwTerminate ();
        return 0;
}
void
error_callback (GLenum source, GLenum type, unsigned int id, GLenum severity,
                GLsizei length, const GLchar *message, const void *userParam)
{
        if (type == GL_DEBUG_TYPE_ERROR)
                {
                        fprintf (stderr,
                                 "GL CALLBACK: %s type = 0x%x, severity = "
                                 "0x%x, message = %s\n",
                                 (type == GL_DEBUG_TYPE_ERROR
                                      ? "** GL ERROR **"
                                      : ""),
                                 type, severity, message);
                }
}
// Matrizes do open gl são acessadas [coluna][linha] ao inverso da notação
// normal A = a[0][0] , a[1][0] , a[2][0], a[3][0]
//     a[0][1] , a[1][1] , a[2][1], a[3][1]
//     a[0][2] , a[1][2] , a[2][2], a[3][2]
//     a[0][3] , a[1][3] , a[2][3], a[3][3]
void
processInput (GLFWwindow *window, int key, int scancode, int action, int mods)

{
        ImGui_ImplGlfw_KeyCallback (window, key, scancode, action, mods);
        if (action == GLFW_PRESS)
                {
                        switch (key)
                                {
                                case GLFW_KEY_ESCAPE:
                                        glfwSetWindowShouldClose (window,
                                                                  true);
                                        break;
                                case GLFW_KEY_UP:
                                        modelMatrix = glm::rotate (
                                            modelMatrix, glm::radians (-10.0f),
                                            glm::vec3 (1, 0, 0));
                                        break;
                                case GLFW_KEY_DOWN:
                                        modelMatrix = glm::rotate (
                                            modelMatrix, glm::radians (2.0f),
                                            glm::vec3 (1, 0, 0));
                                        break;
                                case GLFW_KEY_RIGHT:
                                        modelMatrix = glm::rotate (
                                            modelMatrix, glm::radians (2.0f),
                                            glm::vec3 (0, 1, 0));
                                        break;
                                case GLFW_KEY_LEFT:
                                        modelMatrix = glm::rotate (
                                            modelMatrix, glm::radians (-10.0f),
                                            glm::vec3 (0, 1, 0));
                                        break;
                                case GLFW_KEY_W:
                                        viewMatrix[3][2] += .2;
                                        break;
                                case GLFW_KEY_S:
                                        viewMatrix[3][2] -= .2;
                                        break;
                                case GLFW_KEY_D:
                                        viewMatrix[3][0] += .2;
                                        break;
                                case GLFW_KEY_A:
                                        viewMatrix[3][0] -= .2;
                                        break;
                                default:
                                        break;
                                }
                }

        // viewMatrix[4] += offset;
}

void
framebuffer_size_callback (GLFWwindow *window, int width, int height)
{
        glViewport (0, 0, width, height);
}
// GL CALLBACK:  type = 0x8251, severity = 0x826b, message = Shader Stats:
// SGPRS: 16 VGPRS: 8 Code Size: 52 LDS: 0 Scratch: 0 Max Waves: 20 Spilled
// SGPRs: 0 Spilled VGPRs: 0 PrivMem VGPRs: 0 LSOutputs: 0 HSOutputs: 0
// HSPatchOuts: 0 ESOutputs: 0 GSOutputs: 0 VSOutputs: 0 PSOutputs: 1
// InlineUniforms: 0 DivergentLoop: 0 (PS, W32)
