#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "models.hpp"
#include "shader.hpp"
#include "utils.hpp"
#define PI 3.14
#define VERTEX_SHADERS_LOCALPATH "vShaders.vert"
#define FRAGMENT_SHADERS_LOCALPATH "fShaders.frag"

// 4.6 (Core Profile) Mesa 25.1.3-arch1.3
void framebuffer_size_callback (GLFWwindow *window, int width, int height);
void processInput (GLFWwindow *window);
void error_callback (GLenum source, GLenum type, unsigned int id,
                     GLenum severity, GLsizei length, const GLchar *message,
                     const void *userParam);

// settings
const unsigned int SCR_X = 800;
const unsigned int SCR_Y = 600;

float vertices[] = {
    // positions          // texture coords
    0.5f,  0.5f,  0.0f, // top right
    0.5f,  -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, // top left
};

int
main ()
{
    // getKeywordsId ();
    // model cubo ("assets/3dmodels/Cubo.obj");
    // unsigned int vertexSize = 3 * sizeof (float) * cubo.vertex.size ();
    // unsigned int triangleSize = 3 * sizeof (int) * cubo.triangles.size ();
    // vertices = (float *)malloc (vertexSize);
    // for (int i = 0; i < cubo.vertex.size (); i++)
    //     {
    //         vertices[3 * i] = cubo.vertex[i][0];
    //         vertices[3 * i + 1] = cubo.vertex[i][1];
    //         vertices[3 * i + 2] = cubo.vertex[i][2];
    //     }
    // unsigned int *indices = (unsigned int *)malloc (triangleSize);
    // memcpy ((void *)indices, cubo.triangles.data (), triangleSize);
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int indexSize = sizeof (indices);
    unsigned int indexCount = sizeof (indices) / sizeof (int);
    unsigned int vertexSize = sizeof (vertices);

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



    const GLchar *feedbackVaryings[] = { "outValue" };
    glTransformFeedbackVaryings (shaderProgram, 1, feedbackVaryings,
                                 GL_INTERLEAVED_ATTRIBS);

    glLinkProgram (shaderProgram);
    glUseProgram (shaderProgram);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_X/(float)SCR_Y, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),1, GL_FALSE,glm::value_ptr(proj));

    unsigned int VBO, VAO, EBO, TBO, Query;
    glGenVertexArrays (1, &VAO);
    glGenBuffers (1, &VBO);
    glGenBuffers (1, &EBO);
    glGenBuffers (1, &TBO);
    glGenQueries (1, &Query);

    glBindVertexArray (VAO);

    glBindBuffer (GL_ARRAY_BUFFER, TBO);
    glBufferData (GL_ARRAY_BUFFER, sizeof(float) * indexCount*4, nullptr, GL_STATIC_READ);

    glBindBuffer (GL_ARRAY_BUFFER, VBO);
    glBufferData (GL_ARRAY_BUFFER, vertexSize, vertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray (0);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, indexSize, indices, GL_STATIC_DRAW);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float),
                           (void *)0); // set vec4 position


    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    // glClipControl(GL_LOWER_LEFT,GL_ZERO_TO_ONE);
    while (!glfwWindowShouldClose (window))
        {

            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, TBO);
            glBindBuffer (GL_ARRAY_BUFFER, VBO);
            glBufferSubData (GL_ARRAY_BUFFER, 0, vertexSize, vertices);
            glClearColor (0.07f, 0.13f, 0.17f, 1.0f);
            glClear (GL_COLOR_BUFFER_BIT);
            // glUseProgram (shaderProgram);
            glBindVertexArray (VAO);

            glBeginQuery (GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, Query);
            glBeginTransformFeedback (GL_TRIANGLES);
            glDrawElements (GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
                    glEndTransformFeedback();
            glEndQuery (GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

            glFlush ();

            GLuint primitives;
            glGetQueryObjectuiv (Query, GL_QUERY_RESULT, &primitives);

            GLfloat feedback[4*indexCount];
            glGetBufferSubData (GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                                sizeof (feedback), feedback);

            printf ("%u primitives written!\n\n", primitives);

            for (int i = 0; i < indexCount; i++)
                {
                    printf ("{%f , %f , %f,%f}\n", feedback[4*i],feedback[4*i+1],feedback[4*i+2],feedback[4*i+3]);
                }

            glfwSwapBuffers (window);
            glfwPollEvents ();
            processInput (window);
            // sleep (1);
        }
    glDeleteQueries(1, &Query);
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
