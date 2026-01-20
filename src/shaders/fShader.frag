#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 fragColor;

const uint MATERIAL_SOLID_COLOR = 0u;
const uint MATERIAL_TEXTURE = 1u;

const uint MATERIAL_MAPS_KA = 0u;
const uint MATERIAL_MAPS_KD = 1u;
const uint MATERIAL_MAPS_KS = 2u;
const uint MATERIAL_MAPS_NORMAL = 3u;

const uint SPECIAL_LAYER = 0u;
const uint COMMON_LAYER = 1u;
const uint LAMPS_LAYER = 2u;

struct TextureLocation {
    int handler;
    uint index;
};

struct Material {
    float K[9];
    float Ni, d, bm;
    TextureLocation maps[4];
    uint type;
};

in vec2 g_texCoord;
in vec3 normalVec;
in vec4 worldPos;

uniform mat4 view;

uniform sampler2DArray textures[7];


void main() {
    fragColor = vec4(1, 0, 0, 1);
}
