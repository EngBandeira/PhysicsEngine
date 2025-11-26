#pragma once

struct Shader{
  unsigned int Shaders[3];
  Shader ();
  Shader (const char *shadersPath[3]);
  ~Shader();
  void attach(unsigned int program);
};
