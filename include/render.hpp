#pragma once

#include "model.hpp"
#include "shader.hpp"
#include "vertexArray.hpp"
#include <GLFW/glfw3.h>
#include <cstddef>
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
    unsigned int EBO, FBO_FROM,FBO_TO,RBO, TBO, Query, modelMxSSBO,texToRenderOver,texToShowFrom;
    unsigned int feedbacksize,feedbacknumber,samples=4;

    Camera camera;
    bool transFeed = false;
    std::unique_ptr<VAO> m_VAO;
    unsigned int shaderProgram;
    Shader shader;
    GLFWwindow *glfwWin;

    void start(void(*op1)(),void(*op2)(),void(*op3)());
    void addModels(unsigned short n, Model *data);
    void rmModels(unsigned short n,unsigned short* index);
    Render(glm::mat4 vMatrix,glm::mat4 pjMatrix,GLFWwindow *win);
    ~Render();
    private:
    void once();
    void runTime();
    void newframe();
    void renderDrawing();
    void imguiSetting();
    void input();

};
