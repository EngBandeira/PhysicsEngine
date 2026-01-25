#pragma once
#include "game_object.hpp"
#include "assets.hpp"
#include <asm-generic/errno.h>

#include "shader.hpp"

#include "render_data.hpp"
#include "camera.hpp"


class Render {
public:

    bool normalATIVO = 0;
    float normalV = 1;
    unsigned int FBO_FROM,
                 FBO_TO,
                 RBO,
                 TBO,
                 QUERY,
                 texToRenderOver,
                 texToShowFrom,
                 feedbacksize,
                 feedbacknumber,
                 shaderProgram,
                 samples = 4;

    bool transFeed = false;
    Render(){}
    Render(GLFWwindow *win);
    ~Render();

    GameObject *objects;
    unsigned int objects_count = 0;
    Camera camera;
    unsigned int selected_object;
    Assets assets;
    Render_Data render_data;
    Shader shader;
    GLFWwindow *glfwWin;
    unsigned short getOldIndexOfNew(unsigned short i, unsigned short n,unsigned short *index);
    unsigned short getNewIndexOfOld(unsigned short i, unsigned short n,unsigned short *index);

    unsigned int setAVector(glm::vec3 position,glm::vec3 direction);

    unsigned int addObject(GameObjectGenData genData);
    void rmObject();

    void update();
    void once();
    void input();
    void ui();
    void newframe();
    void draw();

};

extern Render render;
