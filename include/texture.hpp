#pragma once
#include <glm/glm.hpp>
class Texture{
    public:

  // unsigned int renderID,Slot;
  glm::uvec2 position; //x: which text[16], y: z of the buffer
  const char *localPath;
  // unsigned char *localBuffer;
  int width, height, BPP;
  Texture(const char *path);
  Texture(){};
};
