#pragma once
#include "glm/glm.hpp"

struct TextureLocation {
    int handler = 0;
    unsigned int index = 0;
};

enum Material_Type {
    SOLID_COLOR,
    TEXTURE
};

struct MaterialCopy {
    public:
    glm::vec4 K[3]; // Ka Kd Ks
    float Ni, d,bm;
    enum Material_Type type;
    TextureLocation maps[4]; // Ka Kd Ks Normal
};


struct MaterialGenData {
    glm::vec4 K[3];//Ka Kd Ks
    float Ni, d,bm;
    enum Material_Type type = TEXTURE;
    char *maps[4];// Ka Kd Ks Normal
    MaterialGenData(char *map_zero);
    MaterialGenData();

};

class Material {
    public:
    glm::vec4 K[3]; // Ka Kd Ks
    float Ni, d,bm;
    enum Material_Type type;
    unsigned int maps[4]; // Ka Kd Ks Normal
};
