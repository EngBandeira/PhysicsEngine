
#version 460 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

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

layout(std430, binding = 0) buffer modelMxBuffer {
    mat4 modelsMatrix[];
};
layout(std430, binding = 1) buffer TexCoordBuffer {
    vec2 texCoord[];
};
layout(std430, binding = 2) buffer TexIndexBuffer {
    uint indexTexture[];
};
layout(std430, binding = 3) buffer ModelsMaterialsBuffer {
    uint modelsMaterials[];
};
layout(std430, binding = 4) buffer NormalVecsBuffer {
    float normalVecsBuffer[]; // to getway with vec3 limitations
};
layout(std430, binding = 5) buffer NormalVecsIndexBuffer {
    uint normalVecsIndexBuffer[];
};
layout(std430, binding = 6) buffer MaterialsBuffer {
    Material materials[];
};

out flat Material material;
out vec2 g_texCoord;
out flat uint modelParentFrag;
out vec3 normalVec;
out vec4 worldPos;

in uint modelParentGeom[3];

uniform sampler2DArray textures[16];
uniform mat4 projection;
uniform mat4 view;

void main() {

    // vec4 niopaVec = vec4(normalVecsBuffer[3 * normalVecsIndexBuffer[gl_PrimitiveIDIn]],
    //         normalVecsBuffer[3 * normalVecsIndexBuffer[gl_PrimitiveIDIn] + 1],
    //         normalVecsBuffer[3 * normalVecsIndexBuffer[gl_PrimitiveIDIn] + 2], 0);

    vec4 posito[3] = {
            (inverse(view) * inverse(projection) * gl_in[0].gl_Position),
            (inverse(view) * inverse(projection) * gl_in[1].gl_Position),
            (inverse(view) * inverse(projection) * gl_in[2].gl_Position)
        };
    vec4 a = posito[0] - posito[1];
    vec4 b = posito[2] - posito[1];
    normalVec = normalize(cross(a.xyz, b.xyz));

    modelParentFrag = modelParentGeom[1];
    gl_PrimitiveID = gl_PrimitiveIDIn;

    material = materials[modelsMaterials[modelParentGeom[1]]];

    uvec3 inxTex = uvec3(indexTexture[3 * gl_PrimitiveIDIn], indexTexture[3 * gl_PrimitiveIDIn + 1], indexTexture[3 * gl_PrimitiveIDIn + 2]);
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        worldPos = posito[i];
        g_texCoord = texCoord[inxTex[i]];
        EmitVertex();
    }

    EndPrimitive();
}
