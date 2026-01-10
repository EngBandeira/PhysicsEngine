#pragma once
#include "log.hpp"

struct Shader {
    unsigned int Shaders[3];
    Shader ();
    ~Shader();
    void attach(unsigned int program);
};
