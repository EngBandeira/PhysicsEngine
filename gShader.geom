
#version 460 core
#pragma debug(on)
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
// layout (location = 1) out vec3 outColor;
out vec3 gColor;
out vec4 outValue;
out vec2 g_texCoord;
out vec3 norm;

layout(std430, binding = 2) buffer TexCoordBuffer {
    vec2 texCoord[];
};
layout(std430, binding = 3) buffer TexIndexBuffer {
    uvec4 indexTexture[];
};
layout(std430, binding = 4) buffer NormVecBuffer {
    vec3 normalVecs[];
};

layout(std430, binding = 5) buffer NormIndexBuffer {
    uint NormalVecIndex[];
};
// layout (location = 1) in vec3 inColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    //
    vec3 cara = (model * vec4((gl_in[0].gl_Position.xyz + normalVecs[NormalVecIndex[gl_PrimitiveIDIn]]), 1)).xyz;
    uvec4 a = indexTexture[gl_PrimitiveIDIn];

    gl_Position = gl_in[0].gl_Position;
    g_texCoord = texCoord[a.x];
    gl_PrimitiveID = gl_PrimitiveIDIn;
    norm = cara;
    outValue = gl_Position;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    g_texCoord = texCoord[a.y];
    gl_PrimitiveID = gl_PrimitiveIDIn;
    norm = cara;
    outValue = gl_Position;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    g_texCoord = texCoord[a.z];
    gl_PrimitiveID = gl_PrimitiveIDIn;
    norm = cara;
    outValue = gl_Position;
    EmitVertex();
    EndPrimitive();
}
