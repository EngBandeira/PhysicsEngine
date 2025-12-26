#pragma once

#include "model.hpp"
#include "shader.hpp"
#include "vertexArray.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#define TEXTURE_COUNT 7

class RenderData{
    public:
    unsigned short texturesCount[TEXTURE_COUNT];
    std::vector<Model> models;
    unsigned int  verticesCount = 0, verticesIndexCount = 0, textureVerticesCount=0,
                        textureVerticesIndexCount=0,normalVecCount=0,// normalIndexCount = nOfPrimitives = verticesIndexCount /3
                        verticesIndexOffset= 0,textureIndexOffset = 0,normalIndexOffset=0;
    int *renderFlags;//one per Model
    unsigned short *modelsPerTex[TEXTURE_COUNT];
    float *vertices,*matrices,*textureVertices,*normalVec;
    unsigned int *verticesIndex, *matricesIndex,*textureVerticesIndex,*normalIndex,*textureIndxDATA;
    //count(matricesIndex) = count(vertices) = verticesCount


    RenderData();
    void freeRenderData();
};

class Camera
{
    public:
    glm::vec4 *up,*foward,*right;
    glm::mat4 translation, rotation, localTranslation, viewMatrix, projMatrix;
    RenderData renderData;
    unsigned short selectedModelIndex=0;
    glm::vec4 getPosition();
    glm::vec4 getUp();
    glm::vec4 getFoward();
    glm::vec4 getRight();
    Camera(glm::mat4 vMatrix,glm::mat4 pjMatrix);
    glm::mat4 *getNMatrix(unsigned short index);
};

struct ModelGenStruct{
    const char *meshPath, *texPath;
    int renderFlags = 0;
};


class Render{
    public:
    char flags; // abcd efgh: h = Update renderData
    unsigned int EBO, FBO_FROM,FBO_TO,RBO, TBO, Query,
                 modelMxSSBO,texCoordSSBO,texIndexSSBO,textureIndxSSBO,renderFlagsSSBO,normalVecsSSBO,normalVecsIndexSSBO,
                 texUtilitary,//texUtilitary: for copying and resing
                 texToRenderOver,texToShowFrom,texARRAY[16];
    unsigned int feedbacksize,feedbacknumber,samples=4;


    Camera camera;
    bool transFeed = false;
    std::unique_ptr<VAO> m_VAO;
    unsigned int shaderProgram;
    Shader shader;
    GLFWwindow *glfwWin;

    void start(void(*op1)(),void(*op2)(),void(*op3)());
    void addModels(unsigned short n, ModelGenStruct *data);
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
