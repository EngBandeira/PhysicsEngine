#include "draw_group_mananger.hpp"
#include "draw_group.hpp"

#include "render.hpp"
#include "shaders_manager.hpp"

int te[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };


unsigned int Draw_Group_Mananger::draw_group_alredy_exist(Gen_Shader shader, MaterialGenData material_gen, unsigned int *program, unsigned int *material) {
    bool program_existed, material_existed;
    *program = render.shaders_manager.get_program((char*[3]){shader.vertex, shader.geometric, shader.fragment}, &program_existed);
    *material = render.material_manager.get_material(material_gen, &material_existed);
    if(!program_existed)
        return -1;
    if(!material_existed)
        return -1;
    for(unsigned int i = 0; i < groups_count; i++) {
        if(groups[i].material != *material)
            continue;
        if(groups[i].program != *program)
            continue;
        return i;
    }
    return -1;
}


unsigned int Draw_Group_Mananger::get_draw_group(Gen_Shader shader, MaterialGenData material_gen) {
    unsigned int program, material;
    unsigned int i = draw_group_alredy_exist(shader, material_gen, &program, &material);
    if(i != (unsigned int)-1)
        return i;
    groups = (DrawGroup*)realloc(groups, sizeof(DrawGroup) * ++groups_count);
    groups[groups_count - 1] = DrawGroup();
    groups[groups_count - 1].material = material;
    groups[groups_count - 1].program = program;
    groups[groups_count - 1].init();

    render.shaders_manager.programs[program].use();
    Material &material_ = render.material_manager.materials[material];
    if(material_.maps[0] < render.texture_manager.textures_count && render.texture_manager.textures[material_.maps[0]].location.handler){
        glUniform1i(glGetUniformLocation(render.shaderProgram, "material.maps[0].handler"), render.texture_manager.textures[material_.maps[0]].location.handler);
        glUniform1ui(glGetUniformLocation(render.shaderProgram, "material.maps[0].index"), render.texture_manager.textures[material_.maps[0]].location.index);
    }

    glUniform1iv(glGetUniformLocation(render.shaderProgram, "textures"), TEXTURE_HANDLERS_COUNT, te);
    glUniformMatrix4fv(glGetUniformLocation(render.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(render.camera.proj_matrix));

    return groups_count - 1;
}



void Draw_Group_Mananger::init() {
    groups = (DrawGroup*)malloc(0);
}

void Draw_Group_Mananger::free_data() {
    for(unsigned int i = 0; i < groups_count; i++) {
        groups[i].freeData();
    }
    free(groups);
    render.texture_manager.free_data();
    render.shaders_manager.free_data();
    render.material_manager.free_data();
}
