
#version 460 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
// layout (location = 1) out vec3 outColor;
out vec3 gColor;
out vec4 outValue;
out vec2 g_texCoord;
out vec3 normalVec;
out uint modelIndexFrag;
// layout (location = 1) in vec3 inColor;
in uint modelIndexGeom[3];

uniform mat4 projection;
uniform mat4 view;
// uniform mat4 model;

layout(std430, binding = 4) buffer TexCoordBuffer {
    vec2 texCoord[];
};
layout(std430, binding = 5) buffer TexIndexBuffer {
    uvec3 indexTexture[];
};

layout(std430, binding = 7) buffer NormalVecsBuffer {
    vec3 normalVecsBuffer[];
};
layout(std430, binding = 8) buffer NormalVecsIndexBuffer {
    uint normalVecsIndexBuffer[];
};

void main() {
    //
    uvec3 inxTex = indexTexture[gl_PrimitiveIDIn];
    gl_Position = gl_in[0].gl_Position;
    g_texCoord = texCoord[inxTex.x];
    gl_PrimitiveID = gl_PrimitiveIDIn;
    modelIndexFrag = modelIndexGeom[0];
    normalVec = normalVecsBuffer[normalVecsIndexBuffer[gl_PrimitiveIDIn]];

    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    g_texCoord = texCoord[inxTex.y];
    gl_PrimitiveID = gl_PrimitiveIDIn;
    modelIndexFrag = modelIndexGeom[0];
    normalVec = normalVecsBuffer[normalVecsIndexBuffer[gl_PrimitiveIDIn]];

    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    g_texCoord = texCoord[inxTex.z];
    gl_PrimitiveID = gl_PrimitiveIDIn;
    modelIndexFrag = modelIndexGeom[0];
    normalVec = normalVecsBuffer[normalVecsIndexBuffer[gl_PrimitiveIDIn]];

    EmitVertex();
    EndPrimitive();
}
