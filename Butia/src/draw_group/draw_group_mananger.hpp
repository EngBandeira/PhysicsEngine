#pragma once

#include "draw_group.hpp"
#include "shader/shader_manager.hpp"
#include "material/material.hpp"

class Butia;

class Draw_Group_Mananger {
    unsigned int draw_group_alredy_exist(Gen_Shader shader, MaterialGenData material_gen, unsigned int *program, unsigned int *material);
    public:
    DrawGroup *groups;
    unsigned int groups_count = 0;

    unsigned int get_draw_group(Gen_Shader shader, MaterialGenData material_gen);

    void init();
    void free_data();

    Draw_Group_Mananger(){}
};
