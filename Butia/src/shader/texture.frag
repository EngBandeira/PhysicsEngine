#version 460 core
// #extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 fragColor;

const uint MATERIAL_SOLID_COLOR = 0u;
const uint MATERIAL_TEXTURE = 1u;

const uint MATERIAL_MAPS_KD = 0u;
const uint MATERIAL_MAPS_KA = 1u;
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
    vec4 K[3];
    float Ni, d, bm;
    uint type;
    TextureLocation maps[4];
};

layout(std430, binding = 5) buffer MaterialsSSBO {
    Material materials_ssbo[];
};

in vec2 g_texture_vertices;
in vec3 normalVec;
in vec4 worldPos;

uniform mat4 view;

uniform sampler2DArray textures[7];

uniform uint material_index;

void main() {
    Material material = materials_ssbo[material_index];
    fragColor = texture(textures[material.maps[0].handler],
            vec3(g_texture_vertices, material.maps[0].index));
}
