#pragma once
#include "glm/glm.hpp"

struct TextureLocation {
    int handler;
    unsigned int index;
};

enum Material_Type {
    SOLID_COLOR,
    TEXTURE
};

struct MaterialGenData {
    glm::vec3 K[3];//Ka Kd Ks
    float Ni, d,bm;
    char *maps[4];// Ka Kd Ks Normal
    enum Material_Type type = TEXTURE;
    MaterialGenData(char *map_zero);
    MaterialGenData();

};

class Material {
    public:
    float K[3*3]; // Ka Kd Ks
    float Ni, d,bm;
    unsigned int maps[4]; // Ka Kd Ks Normal
    enum Material_Type type;
};
