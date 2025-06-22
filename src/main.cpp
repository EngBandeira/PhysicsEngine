#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <stdio.h>
#include <glm/glm.hpp>

#include <iostream>
// 4.6 (Core Profile) Mesa 25.1.3-arch1.3
void framebuffer_size_callback (GLFWwindow *window, int width, int height);
void processInput (GLFWwindow *window);
void error_callback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam );

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
char *
readFile (FILE *file)
{
    char *buffer;
    if (file != NULL)
        {
            fseek (file, 0L, SEEK_END);
            short sz = ftell (file);
            fseek (file, 0, SEEK_SET);
            buffer = (char *)malloc (sz * sizeof (char));
            fread (buffer, sizeof (char), sz, file);
        }
    else
        {
            printf ("Not able to open the file.");
        }
    fclose (file);
    return buffer;
}

int
main ()
{
    char *shadersbuffers[2] = { readFile (fopen ("vShaders.glsl", "r")),
                                readFile (fopen ("fShaders.glsl", "r")) };

    glfwInit ();
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window
        = glfwCreateWindow (SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate ();
            return -1;
        }
    glfwMakeContextCurrent (window);
    glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);

    if (glewInit ())
        {
            std::cout << "Failed to initialize GLEW" << std::endl;
            return -1;
        }
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(&error_callback, 0);

    unsigned int vertexShader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vertexShader, 1, &shadersbuffers[0], NULL);
    glCompileShader (vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv (vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
        {
            glGetShaderInfoLog (vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

    unsigned int fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fragmentShader, 1, &shadersbuffers[1], NULL);
    glCompileShader (fragmentShader);
    glGetShaderiv (fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
        {
            glGetShaderInfoLog (fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

    unsigned int shaderProgram = glCreateProgram ();
    glAttachShader (shaderProgram, vertexShader);
    glAttachShader (shaderProgram, fragmentShader);
    glLinkProgram (shaderProgram);
    glGetProgramiv (shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
        {
            glGetProgramInfoLog (shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
        0.5f,  -0.5f, 0.0f, // right
        0.0f,  0.5f,  0.0f  // top
    };

    // A Vertex Array Object (VAO) is an object which contains one or more
    // Vertex Buffer Objects A Vertex Buffer Object (VBO) is a memory buffer in
    // the high speed memory of your video card designed to hold information
    // about vertices
    unsigned int VBO, VAO;
    glGenVertexArrays (1, &VAO);
    /* Bind our Vertex Array Object as the current used object */
    glBindVertexArray (VAO); // After this all VAO operations will be do inside
    glGenBuffers (1, &VBO);
    glBindBuffer (GL_ARRAY_BUFFER, VBO);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices,
                  GL_STATIC_DRAW);
    /* Specify that our coordinate data is going into attribute index 0, and
     * contains three floats per vertex */
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float),
                           (void *)0);
    /* Enable attribute index 0 as being used */
    glEnableVertexAttribArray (0);

    // note that this is allowed, the call to glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object so afterwards
    // we can safely unbind
    glBindBuffer (GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally
    // modify this VAO, but this rarely happens. Modifying other VAOs requires
    // a call to glBindVertexArray anyways so we generally don't unbind VAOs
    // (nor VBOs) when it's not directly necessary.
    glBindVertexArray (0);

    glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
    glUseProgram (shaderProgram);
    glBindVertexArray (VAO);
    while (!glfwWindowShouldClose (window))
        {
           
            processInput (window);

            glClear (GL_COLOR_BUFFER_BIT);

            glDrawArrays (GL_TRIANGLES, 0, 3);
            glfwSwapBuffers (window);
            glfwPollEvents ();
        }

    glDeleteShader (vertexShader);
    glDeleteShader (fragmentShader);
    glDeleteVertexArrays (1, &VAO);
    glDeleteBuffers (1, &VBO);
    glDeleteProgram (shaderProgram);
    glfwTerminate ();
    return 0;
}

void error_callback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
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