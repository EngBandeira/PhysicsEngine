#pragma once


struct Gen_Shader {
    char *vertex, *geometric, *fragment;
};

struct Shader {
    char *name;
    unsigned int code;
    char type;
};

class Program {
    public:
    bool have_geometric = 1;
    unsigned int program;
    Shader shaders[3];
    unsigned int compiled_shaders[3];
    void init();
    void free_data();
    void use();
};

class Shaders_Manager {
    unsigned int program_alredy_exist(unsigned int vertex_code,unsigned int geometric_code,unsigned int fragment_code) ;
    public:
    Shader *shaders;
    Program *programs;
    unsigned int shaders_count = 0, programs_count = 0;
    unsigned int get_program(char* shaders_name[3], bool *existed);
    void update();
    void init();
    void free_data();
    Shaders_Manager(){}
};
