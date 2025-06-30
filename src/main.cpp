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
void error_callback (GLenum source, GLenum type, unsigned int id,
                     GLenum severity, GLsizei length, const GLchar *message,
                     const void *userParam);

// settings
const unsigned int SCR_X = 800;
const unsigned int SCR_Y = 600;

float vertices[]
    = { 1.0, 1.0, 5.0, 0.0, 1.0, 5.0, 0.0, 0.0, 5.0, 1.0, 0.0, 5.0 };

int
main ()
{
    glfwInit ();
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window
        = glfwCreateWindow (SCR_X, SCR_Y, "PhysicsEngine", NULL, NULL);
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

    // float vertices[]
    //     = { 0.346623, 0.346623, 0, 0, 0.346623, 0, 0, 0, 0, 0.346623, 0, 0
    //     };
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays (1, &VAO);
    glGenBuffers (1, &VBO);
    glGenBuffers (1, &EBO);

    glBindVertexArray (VAO);

    glBindBuffer (GL_ARRAY_BUFFER, VBO);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices,
                  GL_DYNAMIC_DRAW);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices,
                  GL_STATIC_DRAW);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float),
                           (void *)0); // set vec4 position
    glEnableVertexAttribArray (0);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    // glClipControl(GL_LOWER_LEFT,GL_ZERO_TO_ONE);
    while (!glfwWindowShouldClose (window))
        {

            glBindBuffer (GL_ARRAY_BUFFER, VBO);
            glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof (vertices),
            vertices);
            glClearColor (0.07f, 0.13f, 0.17f, 1.0f);
            glClear (GL_COLOR_BUFFER_BIT);
            glUseProgram (shaderProgram);
            glBindVertexArray (VAO);

            glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glfwSwapBuffers (window);
            glfwPollEvents ();
            processInput (window);
            // sleep (1);
        }

    glDeleteVertexArrays (1, &VAO);
    glDeleteBuffers (1, &VBO);
    glDeleteBuffers (1, &EBO);
    glDeleteProgram (shaderProgram);
    glfwDestroyWindow (window);
    glfwTerminate ();
    return 0;
}
void
error_callback (GLenum source, GLenum type, unsigned int id, GLenum severity,
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

    if (glfwGetKey (window, GLFW_KEY_W) == GLFW_PRESS)
        {
            for (int i = 0; i < 4; i++)
                {
                    vertices[3 * i + 2] += .1;
                }
        }
    if (glfwGetKey (window, GLFW_KEY_S) == GLFW_PRESS)
        {
            for (int i = 0; i < 4; i++)
                {
                    vertices[3 * i + 2] -= .1;
                }
        }
    if (glfwGetKey (window, GLFW_KEY_D) == GLFW_PRESS)
        {
            for (int i = 0; i < 4; i++)
                {
                    vertices[3 * i] += 0.1;
                }
        }
    if (glfwGetKey (window, GLFW_KEY_A) == GLFW_PRESS)
        {
            for (int i = 0; i < 4; i++)
                {
                    vertices[3 * i] -= 0.1;
                }
        }
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
