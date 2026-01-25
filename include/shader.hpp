#pragma once
#include "log.hpp"

struct Shader {
    unsigned int shaders[3];
    void init(const char *vertex_shaders, const char *geometry_shaders, const char *fragment_shaders);
    void free_data();
    void attach(unsigned int program);
};
