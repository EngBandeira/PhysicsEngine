#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <stdio.h>

#include <iostream>

const unsigned int SCREEN_X = 800;
const unsigned int SCREEN_Y = 600;

int
main ()
{
    FILE *f[2];
    f[0] = fopen ("vShaders.glsl", "r");
    f[1] = fopen ("fShaders.glsl", "r");
    char *shadersbuffers[2];
    for (int i = 0; i < 2; i++)
        {
            if (f[i] != NULL)
                {
                    fseek (f[i], 0L, SEEK_END);
                    short sz = ftell (f[i]);
                    fseek (f[i], 0, SEEK_SET);
                    shadersbuffers[i] = (char *)malloc (sz * sizeof (char));
                    fread (shadersbuffers[i], sizeof (char), sz, f[i]);
                }
            else
                {
                    printf ("Not able to open the file.");
                }
            fclose (f[i]);
        }

    glfwInit ();
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window
        = glfwCreateWindow (SCREEN_X, SCREEN_Y, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate ();
            return -1;
        }
    glfwMakeContextCurrent (window);
    glfwSetFramebufferSizeCallback (
        window, [] (GLFWwindow *window, int width, int height) { glViewport (0, 0, width, height); });

    if (glewInit ())
        {
            std::cout << "Failed to initialize GLEW" << std::endl;
            return -1;
        }

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
    glUseProgram (shaderProgram);

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

    unsigned int VBO, VAO;
    glGenVertexArrays (1, &VAO);
    glBindVertexArray (VAO);

    glGenBuffers (1, &VBO);
    glBindBuffer (GL_ARRAY_BUFFER, VBO);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices,
                  GL_STATIC_DRAW);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float),
                           (void *)0);
    glEnableVertexAttribArray (0);

    glBindBuffer (GL_ARRAY_BUFFER, 0);

    glBindVertexArray (0);

    glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
    while (!glfwWindowShouldClose (window))
        {
            if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose (window, true);

            glClear (GL_COLOR_BUFFER_BIT);

            glBindVertexArray (VAO);
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
