
#version 460 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

struct TextureLocation {
    int handler;
    uint index;
};

const uint MATERIAL_SOLID_COLOR = 0u;
const uint MATERIAL_TEXTURE = 1u;

const uint MATERIAL_MAPS_KA = 0u;
const uint MATERIAL_MAPS_KD = 1u;
const uint MATERIAL_MAPS_KS = 2u;
const uint MATERIAL_MAPS_NORMAL = 3u;

struct Material {
    float K[9];
    float Ni, d, bm;
    TextureLocation maps[4];
    uint type;
};

layout(std430, binding = 0) buffer TextureVerticesSSBO {
    vec2 texture_vertices[];
};

layout(std430, binding = 1) buffer TextureIndexSSBO {
    uint texture_vertices_index[];
};
layout(std430, binding = 4) buffer TextureVerticesOffsetSSBO {
    uint texture_vertices_offset_ssbo[];
};

uniform sampler2DArray textures[7];
uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_PrimitiveID = gl_PrimitiveIDIn;
    vec4 pinto[4];
    pinto[0] = vec4(.5, .5, 0, 0);
    pinto[1] = vec4(-.5, .5, 0, 0);
    pinto[2] = vec4(.5, -.5, 0, 0);
    pinto[3] = vec4(-.5, -.5, 0, 0);
    for (int i = 0; i < 4; i++) {
        gl_Position = gl_in[0].gl_Position + pinto[i];
        EmitVertex();
    }

    EndPrimitive();
}
