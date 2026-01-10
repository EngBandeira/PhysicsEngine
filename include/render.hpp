#pragma once
#include "common.hpp"

#include <GLFW/glfw3.h>
#include <asm-generic/errno.h>
#include <vector>

#include "model.hpp"
#include "log.hpp"
#include "shader.hpp"

#include "render/render_data.hpp"
#include "render/camera.hpp"
#include "render/ui.hpp"



struct File {
    char *simpleName; // complete name = simpleName + '.' + extension
    char *extension;
    char *completeName;
    FILE_TYPES type = FILE_TYPES::COMMON_FILE;
};

struct Assets {
    std::vector<File> files;
    unsigned int framesDelay = FRAMES_DELAY;
    char *directory = nullptr;
    void update();
    unsigned int delayCounter = 0;
};



class Render {
    public:
    char flags = 0;
    unsigned int FBO_FROM, FBO_TO, RBO, TBO, QUERY,
                 texToRenderOver,texToShowFrom;
    unsigned int feedbacksize,feedbacknumber, samples=4;

    Camera camera;
    bool transFeed = false;

    void start(void(*op1)(),void(*op2)(),void(*op3)());

    Render(GLFWwindow *win);
    ~Render();


    Assets assets;
    RenderData renderData;
    LAYER selectedModelLayer=LAYER::COMMON_LAYER;
    unsigned int selectedModelIndex=0;
    unsigned int shaderProgram;
    Shader shader;
    GLFWwindow *glfwWin;
    unsigned short getOldIndexOfNew(unsigned short i, unsigned short n,unsigned short *index);
    unsigned short getNewIndexOfOld(unsigned short i, unsigned short n,unsigned short *index);

    void getProgramStatus(unsigned int shaderProgram,int status);
    void getShaderStatus(unsigned int shaderProgram,int status);

    unsigned int setAVector(glm::vec3 positon,glm::vec3 direction);

    void updatePipeline(unsigned int layerIndex);
    void once();
    void input();
    void ui();
    void update();
    void newframe();
    void renderDrawing(unsigned int layerIndex);

};
