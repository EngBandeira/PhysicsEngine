#pragma once

#include <vector>


struct Shader{
  unsigned int shader;
  Shader();
  Shader (char *localPath, GLenum shadersType);
  ~Shader();
  void attach(unsigned int program);
};
