#pragma once
#include "draw_group_mananger.hpp"
#include "game_object.hpp"
#include "assets.hpp"
#include <asm-generic/errno.h>

#include "material_mananger.hpp"
#include "render_data.hpp"
#include "camera.hpp"
#include "scripts_mananger.hpp"
#include "shaders_manager.hpp"
#include "texture_manager.hpp"


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
                 tex_post,
                 feedbacksize,
                 feedbacknumber,
                 shaderProgram,
                 post_program,
                 samples = 4;

    bool transFeed = false;
    Render(){}
    Render(GLFWwindow *win);
    void free_data();

    struct default_game_objects {
        unsigned int seta, grid;
    } default_game_objects;

    GameObject *objects;
    unsigned int objects_count = 0;
    Camera camera;
    unsigned int selected_object = 0;
    Assets assets;
    Render_Data render_data;
    Scripts_Manager scripts_manager;
    Draw_Group_Mananger draw_group_manager;
    Shaders_Manager shaders_manager;
    Material_Manager material_manager;
    Texture_Mananger texture_manager;
    GLFWwindow *glfwWin;
    unsigned short getOldIndexOfNew(unsigned short i, unsigned short n,unsigned short *index);
    unsigned short getNewIndexOfOld(unsigned short i, unsigned short n,unsigned short *index);

    unsigned int setAVector(glm::vec3 position,glm::vec3 direction);

    unsigned int addObject(GameObjectGenData genData);
    unsigned int addObject(GameObjectGenData genData, unsigned int draw_group);
    unsigned int addObject(const char* mesh, const char* name, unsigned int draw_group);
    void rmObject();

    void update();
    void once();
    void input();
    void ui();
    void post_processing();
    void newframe();
    void draw();

};

extern Render render;
