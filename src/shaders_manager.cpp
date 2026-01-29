
#include "shaders_manager.hpp"
#include "common.hpp"
#include "render.hpp"
#include "utils.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

void Program::init() {

    program = glCreateProgram();

    unsigned int sets[3]
        = { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER };

    for (int i = 0; i < 3; i++)
    {
        if(i == 1 && !have_geometric)
            continue;
        char *path = (char*)malloc(14 + strlen(shaders[i].name));
        sprintf(path, "src/shaders/%s",shaders[i].name);
        char *file = utils.read_file (path, nullptr);
        compiled_shaders[i] = glCreateShader (sets[i]);
        glShaderSource (compiled_shaders[i], 1, &file, NULL);//aq
        glCompileShader (compiled_shaders[i]);

        free (file);

        utils.get_shader_status(compiled_shaders[i], GL_COMPILE_STATUS);
    }
    glAttachShader (program, compiled_shaders[0]);
    if(have_geometric)
        glAttachShader (program, compiled_shaders[1]);
    glAttachShader (program, compiled_shaders[2]);

    glLinkProgram(program);
    utils.get_program_status(program,GL_LINK_STATUS);
}

void Program::use() {
    glUseProgram(program);
    render.shaderProgram = program;
}

void Program::free_data()
{
    glDeleteProgram(program);
    glDeleteShader (compiled_shaders[0]);
    if(have_geometric)
        glDeleteShader (compiled_shaders[1]);
    glDeleteShader (compiled_shaders[2]);
}






unsigned int Shaders_Manager::program_alredy_exist(unsigned int vertex_code,unsigned int geometric_code,unsigned int fragment_code) {
    for(unsigned int i = 0; i < programs_count; i++) {
        if( programs[i].shaders[0].code == vertex_code &&
            programs[i].shaders[1].code == geometric_code &&
            programs[i].shaders[2].code == fragment_code)
                return i;
    }
    return -1;
}

unsigned int Shaders_Manager::get_program(char* shaders_name[3], bool *existed) {
    Program program;
    for(int i = 0; i < 3; i++) {
        program.shaders[i].name = shaders_name[i];
        if(i == 1 && shaders_name[i] == nullptr){
            program.have_geometric = 0;
            program.shaders[i].code = 0;
            continue;
        }
        program.shaders[i].code = utils.sha256(shaders_name[i]);
    }

    unsigned int i = program_alredy_exist(program.shaders[0].code, program.shaders[1].code, program.shaders[2].code);
    if(i != (unsigned int)-1){
        *existed = 1;
        return i;
    }
    programs = (Program*)realloc(programs,sizeof(Program) * ++programs_count);
    programs[programs_count - 1] = program;
    programs[programs_count - 1].init();
    *existed = 0;
    return programs_count - 1;
}

void Shaders_Manager::init() {
    shaders = (Shader*)malloc(0);
    programs = (Program*)malloc(0);

    assets.directory = (char*)"src/shaders";
    assets.init();
    update();
}

void Shaders_Manager::update() {
    assets.update();
    shaders_count = 0;
    for(unsigned int i = 1; i < assets.files_number; i++) {
        if(assets.files[i].extension == nullptr)
            continue;
        if(assets.files[i].extension[0] == 'v' ||
            assets.files[i].extension[0] == 'g' ||
            assets.files[i].extension[0] == 'f')
        shaders_count++;
    }
    shaders = (Shader*)realloc(shaders, sizeof(Shader) * shaders_count);
    unsigned int shader_i = 0;
    for(unsigned int i = 1; i < assets.files_number; i++) {
        if( assets.files[i].extension[0] == 'v' ||
            assets.files[i].extension[0] == 'g' ||
            assets.files[i].extension[0] == 'f') {
                shaders[shader_i].type = assets.files[i].extension[0];
                shaders[shader_i].name = assets.files[i].completeName;
                shaders[shader_i].code = utils.sha256(assets.files[i].completeName);
                shader_i++;
        }
    }
}
void Shaders_Manager::free_data() {
    assets.free_data();
    for(unsigned int i = 0; i < programs_count; i++) {
        programs[i].free_data();
    }
    free(programs);
    free(shaders);
}
