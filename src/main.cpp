#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <glm/glm.hpp>
#include <iostream>
#include <stdio.h>

#include "models.hpp"
#include "shader.hpp"
#include "utils.hpp"
#define PI 3.14
#define VERTEX_SHADERS_LOCALPATH "vShaders.glsl"
#define FRAGMENT_SHADERS_LOCALPATH "fShaders.glsl"

// 4.6 (Core Profile) Mesa 25.1.3-arch1.3
void framebuffer_size_callback (GLFWwindow *window, int width, int height);
void processInput (GLFWwindow *window);
void error_callback (GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message,
                     const void *userParam);

// settings
const unsigned int SCR_X = 800;
const unsigned int SCR_Y = 600;

int
main ()
{
    // model m((char*)"assets/3dmodels/Cubo.obj");

    glfwInit ();
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window
        = glfwCreateWindow (SCR_X, SCR_Y, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
        {
            sendError ("failed to crate a Glfw Window");
            glfwTerminate ();
            return -1;
        }
    glfwMakeContextCurrent (window);
    glfwSetFramebufferSizeCallback (
        window, [] (GLFWwindow *window, int width, int height) {
            glViewport (0, 0, width, height);
        });

    if (glewInit ())
        {
            sendError ("cannot init glew");
            return -1;
        }
    glEnable (GL_DEBUG_OUTPUT);
    glDebugMessageCallback (&error_callback, 0);

    Shader vertexShader (VERTEX_SHADERS_LOCALPATH, GL_VERTEX_SHADER);
    Shader fragmentShader (FRAGMENT_SHADERS_LOCALPATH, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram ();
    vertexShader.attach (shaderProgram);
    fragmentShader.attach (shaderProgram);
    glLinkProgram (shaderProgram);

    glUseProgram (shaderProgram);

    float vertices[] = {1.0, 1.0, 5.0, 0.0, 1.0, 5.0,
                        0.0, 0.0, 5.0 ,1.0, 0.0, 5.0 };
    unsigned int indeces[] = { 0, 1, 2, 1, 2, 3 };
    unsigned int buffer, VAO, ibo;

    glGenVertexArrays (1, &VAO);
    glGenBuffers (1, &buffer);
    glGenBuffers (1, &ibo);

    glBindVertexArray (VAO);
    glBindBuffer (GL_ARRAY_BUFFER, buffer);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices,
                  GL_STATIC_DRAW);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indeces), indeces,
                  GL_STATIC_DRAW);

    glEnableVertexAttribArray (0);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float),
                           (void *)0);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);

    glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
    while (!glfwWindowShouldClose (window))
        {


            glClear (GL_COLOR_BUFFER_BIT);
            glUseProgram (shaderProgram);
            glBindVertexArray (VAO);
            processInput (window);
            // glDrawArrays(GL_TRIANGLES,0,3);
            glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
            glfwSwapBuffers (window);
            glfwPollEvents ();
        }
    glDeleteVertexArrays (1, &VAO);
    glDeleteBuffers (1, &buffer);
    glDeleteBuffers (1, &ibo);
    glDeleteProgram (shaderProgram);
    glfwTerminate ();
    return 0;
}
void
error_callback (GLenum source, GLenum type, GLuint id, GLenum severity,
                GLsizei length, const GLchar *message, const void *userParam)
{
    if (type == GL_DEBUG_TYPE_ERROR)
        {
            fprintf (
                stderr,
                "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
                severity, message);
        }
}
void
processInput (GLFWwindow *window)
{
    if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose (window, true);
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