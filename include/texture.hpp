#pragma once
#include <glm/glm.hpp>

// struct Material{
//     char *name;
//     int flags,illum;
//     glm::vec3 K[3];//Ka Ks Ke
//     float Ni, d,bm;
//     char *maps[4];// Kd Ns refl Bump
//     void deleteMaterial();
// };
enum materialType{
    SOLID_COLOR,
    TEXTURE
};
class Material{
    public:
    char *name;
    int flags,illum;
    glm::vec3 K[3];//Ka Kd Ks
    float Ni, d,bm;
    char *maps[4];// Kd Ns refl Bump
    enum materialType type;
    void deleteMaterial();
};


Material *mtl(const char *path,unsigned int *materialsCount);

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
