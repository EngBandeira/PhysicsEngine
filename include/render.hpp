#pragma once

#include "model.hpp"
#include "shader.hpp"
#include "vertexArray.hpp"
#include <GLFW/glfw3.h>
#include <vector>

class RenderData{
    public:
    std::vector<Model> models;
    unsigned int verticesCount = 0, verticesIndexCount = 0,verticesOffsetIndex= 0;
    float *vertices,*matrices;
    unsigned int *verticesIndex, *matricesIndex;
    //count(matricesIndex) = count(vertices) = verticesCount

    int *verticesPerModels;


    RenderData();
    ~RenderData();
};

class Camera
{
    public:
    glm::mat4 viewMatrix,projMatrix;
    RenderData renderData;
    unsigned short selectedModelIndex=1;
    Camera(glm::mat4 vMatrix,glm::mat4 pjMatrix);
    glm::mat4 *getNMatrix(unsigned short index);
};

class Render{
    public:
    char flags; // abcd efgh: h = Update renderData
    unsigned int EBO, FBO,RBO, TBO, Query, modelMxSSBO,renderTex;
    unsigned int feedbacksize,feedbacknumber;

    Camera camera;
    bool transFeed = false;
    std::unique_ptr<VAO> m_VAO;
    unsigned int shaderProgram;
    Shader shader;
    GLFWwindow *glfwWin;

    void predraw();
    void newframe();
    void draw();
    void addModels(unsigned short n, Model *data);
    void rmModels(unsigned short n,unsigned short* index);
    void input();
    Render(glm::mat4 vMatrix,glm::mat4 pjMatrix,GLFWwindow *win);
    ~Render();
};
