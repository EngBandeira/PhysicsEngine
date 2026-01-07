#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 fragColor;

struct TextureLocation {
    uint handler, index;
};

const uint MATERIAL_SOLID_COLOR = 0u;
const uint MATERIAL_TEXTURE = 1u;

struct Material {
    float K[9];
    float Ni, d, bm;
    TextureLocation maps[4];
    uint type;
};

in flat uint modelParentFrag;
in flat Material material;
in vec2 g_texCoord;

uniform sampler2DArray textures[16];

void main() {
    fragColor = vec4(1, 0, 0, 1);
    if (material.type == MATERIAL_TEXTURE) {
        fragColor = texture(textures[material.maps[0].handler],
                vec3(g_texCoord, material.maps[0].index));
    }
    else {
        fragColor = vec4(1, 0, 0, 1);
    }
}
