#pragma once

#include <vector>

struct Uniform{
    char *type, *name;
    ~Uniform();
};

struct Shader{
  unsigned int shader;
  
  std::vector<Uniform> uniforms;
  Shader();
  Shader (char *localPath, GLenum shadersType);
  ~Shader();
  void linkUniform(char *name);
  void attach(unsigned int program);
};
