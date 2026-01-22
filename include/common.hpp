#pragma once
#include "log.hpp"
#include "utils.hpp"
#include "glm/glm.hpp"

extern glm::vec3 axis[3];
extern Log logger;
extern Utils utils;

enum AXIS { X, Y, Z };

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION

static char flags = 0;

const char VERTEX_SHADERS_LOCALPATH[]   = "src/shaders/vShader.vert";
const char FRAGMENT_SHADERS_LOCALPATH[] = "src/shaders/fShader.frag";
const char GEOMETRY_SHADERS_LOCALPATH[] = "src/shaders/gShader.geom";
const char RAW_TEX_EXTENSION[] =  "raw";

const int TEXTURE_HANDLERS_COUNT = 7;
const int SSBOS_COUNT            = 2;
const int LEVEL                  = 0;
const double MOUSE_SENSI         = 0.05f;

const double MAX_RATIO_OF_EMPTY_TEXTURES  = .5;
const int MIN_EMPTY_TEXTURES_COUNT_TO_RST = 3;
const int MAX_EMPTY_TEXTURES_COUNT        = 10;

const int MATERIAL_CHANGE_FLAG = 1 << 0;
const int MATRICES_CHANGE_FLAG = 1 << 1;
const int MODELS_CHANGE_FLAG   = 1 << 2;
const int LAMPS_CHANGE_FLAG    = 1 << 3;

// Render defines
const int SCR_X = 1920;
const int SCR_Y = 1080;

const int MAX_FILE_EXTENSION_LENGHT = 6;
const int RAW_TEX_EXTENSION_LENGHT  = 3;

const int ASSETS_DELAY = 120;

enum LAYER {
    SPECIAL_LAYER,
    COMMON_LAYER,
    LAMPS,
};

enum MATERIAL_MAPS {
    KA,
    KD,
    KS,
    NORMAL
};

enum VertexCase {
  COMMON_CASE,
  LAMP_CASE
};

enum SSBOS {
    TextureVerticesSSBO,
    TextureIndexSSBO,
};

enum FILE_TYPES {
    COMMON_FILE,
    FOLDER_FILE,
    MESH_FILE,
    IMAGE_FILE,
    CODE_FILE
};
